/*
 * 和時計（不定時法）ウォッチフェイス
 *
 * 文字盤の作りは JR Mobley の Horizon Watch Face（CC BY-SA 4.0）による。
 * 24 時間で一周する太陽の軌道を、南中が真上に来るよう回し、
 * 日の出と日の入りを結ぶ線を地平線として空と地を塗り分ける。
 * 時刻の目盛りは、昼と夜をそれぞれ六等分した不定時法の刻に置き換えている。
 */

#include <pebble.h>
#include "wadokei.h"

// --------------------------------------------------------------------------
// 定数
// --------------------------------------------------------------------------

typedef enum {
    PersistKeyLatitude = 1,
    PersistKeyLongitude,
    PersistKeyBasis,
    PersistKeyTimezone,
} PersistKey;

/* 位置情報が届くまでは東京で計算する（1e-4 度単位） */
#define DEFAULT_LATITUDE  356895
#define DEFAULT_LONGITUDE 1396917
#define DEFAULT_TIMEZONE  540     /* 時計にタイムゾーンが無いときの時差（分） */

#if PBL_DISPLAY_WIDTH == 144
#define FONT_JP          RESOURCE_ID_JP_14
#define FONT_BRUSH       RESOURCE_ID_BRUSH_24
#define FONT_BRUSH_SMALL RESOURCE_ID_BRUSH_18
#define FONT_KEY_TIME    FONT_KEY_LECO_20_BOLD_NUMBERS
#elif PBL_DISPLAY_WIDTH == 180
#define FONT_JP          RESOURCE_ID_JP_16
#define FONT_BRUSH       RESOURCE_ID_BRUSH_28
#define FONT_BRUSH_SMALL RESOURCE_ID_BRUSH_22
#define FONT_KEY_TIME    FONT_KEY_LECO_20_BOLD_NUMBERS
#elif PBL_DISPLAY_WIDTH == 200
#define FONT_JP          RESOURCE_ID_JP_18
#define FONT_BRUSH       RESOURCE_ID_BRUSH_32
#define FONT_BRUSH_SMALL RESOURCE_ID_BRUSH_26
#define FONT_KEY_TIME    FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM
#else
#define FONT_JP          RESOURCE_ID_JP_22
#define FONT_BRUSH       RESOURCE_ID_BRUSH_40
#define FONT_BRUSH_SMALL RESOURCE_ID_BRUSH_30
#define FONT_KEY_TIME    FONT_KEY_LECO_32_BOLD_NUMBERS
#endif

#if !defined(PBL_ROUND)
static const char* const kWeekdays[7] = { "日曜", "月曜", "火曜", "水曜", "木曜", "金曜", "土曜" };
#endif

// --------------------------------------------------------------------------
// 状態
// --------------------------------------------------------------------------

static struct {
    Window* window;
    Layer* layer;
    GFont jp;
    GFont brush;
    GFont brushSmall;
    GFont timeFont;

    int32_t latitude;   /* 1e-4 度 */
    int32_t longitude;
    bool edo;           /* 六つを江戸の暦法（太陽の伏角 7°21′40″）で取る */
    int32_t timezone;   /* スマホから届いた時差（分） */

    struct tm now;
    WaDay day;
    int dayKey;         /* day を計算した日（年*1000+通日） */
    int32_t dayOffset;  /* day を計算したときの時差（分） */
} g;

// --------------------------------------------------------------------------
// 色
// --------------------------------------------------------------------------

#define COLOR_SKY        PBL_IF_COLOR_ELSE(GColorPastelYellow, GColorWhite)
#define COLOR_GROUND     PBL_IF_COLOR_ELSE(GColorOxfordBlue, GColorBlack)
#define COLOR_SKY_INK    GColorBlack
#define COLOR_GROUND_INK GColorWhite
#define COLOR_CURRENT    PBL_IF_COLOR_ELSE(GColorOrange, GColorLightGray)
#define COLOR_SUN        PBL_IF_COLOR_ELSE(GColorRed, GColorWhite)
#define COLOR_READOUT    GColorWhite
#define COLOR_ETO        PBL_IF_COLOR_ELSE(GColorDarkCandyAppleRed, GColorBlack)
#define COLOR_SUBTEXT    PBL_IF_COLOR_ELSE(GColorDarkGray, GColorBlack)

// --------------------------------------------------------------------------
// 計算
// --------------------------------------------------------------------------

static float nowMinute(void) {
#ifdef DEMO_MINUTE
    return DEMO_MINUTE;
#else
    return g.now.tm_hour * 60 + g.now.tm_min + g.now.tm_sec / 60.0f;
#endif
}

static int nowYday(void) {
#ifdef DEMO_YDAY
    return DEMO_YDAY;
#else
    return g.now.tm_yday;
#endif
}

static void updateDay(bool force) {
#ifdef DEMO_MINUTE
    int32_t offset = DEFAULT_TIMEZONE;
#else
    int32_t offset = clock_is_timezone_set() ? g.now.tm_gmtoff / 60 : g.timezone;
#endif
    int key = g.now.tm_year * 1000 + nowYday();
    if (!force && key == g.dayKey && offset == g.dayOffset) {
        return;
    }
    g.dayKey = key;
    g.dayOffset = offset;
    wadokei_day(nowYday(), g.latitude / 10000.0f, g.longitude / 10000.0f,
                offset, g.edo, &g.day);
}

/* 文字盤上の角度。南中が真上で、時計回りに進む。 */
static int32_t orbitAngle(float minute) {
    int32_t a = (int32_t)((minute - g.day.noon) * TRIG_MAX_ANGLE / 1440.0f);
    a %= TRIG_MAX_ANGLE;
    if (a < 0) a += TRIG_MAX_ANGLE;
    return a;
}

static GPoint orbitPoint(GPoint c, int16_t r, float minute) {
    int32_t a = orbitAngle(minute);
    return GPoint(c.x + sin_lookup(a) * r / TRIG_MAX_RATIO,
                  c.y - cos_lookup(a) * r / TRIG_MAX_RATIO);
}

// --------------------------------------------------------------------------
// 描画
// --------------------------------------------------------------------------

/* 文字の見た目の中心を p に合わせる */
static void drawCentered(GContext* ctx, const char* text, GFont font, GPoint p, GColor color) {
    GRect box = GRect(0, 0, 200, 60);
    GSize size = graphics_text_layout_get_content_size(
        text, font, box, GTextOverflowModeWordWrap, GTextAlignmentLeft);
    box.origin.x = p.x - size.w / 2;
    box.origin.y = p.y - size.h * 5 / 8;
    box.size.w = size.w + 2;
    box.size.h = size.h + 4;
    graphics_context_set_text_color(ctx, color);
    graphics_draw_text(ctx, text, font, box, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

#if !defined(PBL_ROUND)
static void drawAligned(GContext* ctx, const char* text, GFont font, GRect box,
                        GTextAlignment align, GColor color) {
    graphics_context_set_text_color(ctx, color);
    graphics_draw_text(ctx, text, font, box, GTextOverflowModeFill, align, NULL);
}
#endif

static void formatTime(char* buf, size_t len) {
    int minute = (int)nowMinute();
    int hour = minute / 60;
    if (!clock_is_24h_style()) {
        hour %= 12;
        if (hour == 0) hour = 12;
        snprintf(buf, len, "%d:%02d", hour, minute % 60);
    } else {
        snprintf(buf, len, "%02d:%02d", hour, minute % 60);
    }
}

static void drawClock(Layer* layer, GContext* ctx) {
    GRect bounds = layer_get_unobstructed_bounds(layer);
    GPoint center = grect_center_point(&bounds);
    int16_t inner = bounds.size.w < bounds.size.h ? bounds.size.w : bounds.size.h;

    int16_t R = inner / 2 - PBL_IF_ROUND_ELSE(2, 0);
    int16_t discR = R / 8;
    int16_t orbitR = R - discR - 1;
    int16_t readoutR = orbitR - discR - 4;

    float now = nowMinute();
    Koku koku;
    wadokei_koku(&g.day, now, &koku);

    graphics_context_set_antialiased(ctx, true);

    /* --- 空と地 --- */
    GPoint rise = orbitPoint(center, orbitR, g.day.sunrise);
    GPoint set = orbitPoint(center, orbitR, g.day.sunset);
    int16_t horizonY = (rise.y + set.y + 1) / 2;

    GRect sky = bounds;
    sky.size.h = horizonY - bounds.origin.y;
    graphics_context_set_fill_color(ctx, COLOR_SKY);
    graphics_fill_rect(ctx, sky, 0, GCornerNone);

    GRect ground = bounds;
    ground.origin.y = horizonY;
    ground.size.h = bounds.origin.y + bounds.size.h - horizonY;
    graphics_context_set_fill_color(ctx, COLOR_GROUND);
    graphics_fill_rect(ctx, ground, 0, GCornerNone);

    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(bounds.origin.x, horizonY),
                       GPoint(bounds.origin.x + bounds.size.w, horizonY));

    /* --- いまの刻を軌道上に帯で示す --- */
    int16_t band = discR * 2 - 2;
    GRect bandRect = GRect(center.x - orbitR - band / 2, center.y - orbitR - band / 2,
                           orbitR * 2 + band, orbitR * 2 + band);
    int32_t a0 = orbitAngle(koku.start);
    int32_t a1 = a0 + (int32_t)(koku.length * TRIG_MAX_ANGLE / 1440.0f);
    graphics_context_set_fill_color(ctx, COLOR_CURRENT);
    graphics_fill_radial(ctx, bandRect, GOvalScaleModeFitCircle, band, a0, a1);

    /* --- 半刻の点 --- */
    int16_t pipR = discR / 4 > 1 ? discR / 4 : 1;
    for (int k = 0; k < KOKU_COUNT; ++k) {
        float a = wadokei_boundary(&g.day, k);
        float b = wadokei_boundary(&g.day, (k + 1) % KOKU_COUNT);
        if (b < a) b += 1440.0f;
        GPoint p = orbitPoint(center, orbitR, (a + b) / 2);
        bool inCurrent = (k == koku.index);
        graphics_context_set_fill_color(ctx,
            (inCurrent || p.y < horizonY) ? COLOR_SKY_INK : COLOR_GROUND_INK);
        graphics_fill_circle(ctx, p, pipR);
    }

    /* --- 刻の境目（数字の入った丸） --- */
    for (int k = 0; k < KOKU_COUNT; ++k) {
        GPoint p = orbitPoint(center, orbitR, wadokei_boundary(&g.day, k));
        bool mutsu = (k % 6) == 0;
        GColor fill = mutsu ? GColorBlack : GColorWhite;
        GColor ink = mutsu ? GColorWhite : GColorBlack;
        graphics_context_set_fill_color(ctx, fill);
        graphics_fill_circle(ctx, p, discR);
        graphics_context_set_stroke_color(ctx, ink);
        graphics_draw_circle(ctx, p, discR);
        if (!mutsu) {
            graphics_context_set_stroke_color(ctx, GColorBlack);
            graphics_draw_circle(ctx, p, discR);
        }
        drawCentered(ctx, wadokei_numeral(k), g.jp, p, ink);
    }

    /* --- 太陽 --- */
    GPoint sun = orbitPoint(center, orbitR, now);
    int16_t sunR = discR - 1;
#ifdef PBL_BW
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, sun, sunR + 2);
#endif
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, sun, sunR);
    graphics_context_set_fill_color(ctx, COLOR_SUN);
    graphics_fill_circle(ctx, sun, sunR - 2);

    /* --- 中央の読み取り円 --- */
    graphics_context_set_fill_color(ctx, COLOR_READOUT);
    graphics_fill_circle(ctx, center, readoutR);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_circle(ctx, center, readoutR);
    graphics_context_set_stroke_width(ctx, 1);

    char buf[32];

    /* 干支 */
    snprintf(buf, sizeof(buf), "%sの刻", wadokei_eto(koku.index));
    drawCentered(ctx, buf, g.jp, GPoint(center.x, center.y - readoutR * 56 / 100), COLOR_ETO);

    /* 刻の名 */
    snprintf(buf, sizeof(buf), "%s%s", wadokei_name(koku.index), koku.half ? "半" : "");
    GFont nameFont = g.brush;
    GSize nameSize = graphics_text_layout_get_content_size(
        buf, nameFont, GRect(0, 0, 300, 60), GTextOverflowModeWordWrap, GTextAlignmentLeft);
    if (nameSize.w > readoutR * 2 - 6) {
        nameFont = g.brushSmall;
    }
    drawCentered(ctx, buf, nameFont, GPoint(center.x, center.y - readoutR * 14 / 100), GColorBlack);

    /* 現代の時刻 */
    formatTime(buf, sizeof(buf));
    drawCentered(ctx, buf, g.timeFont, GPoint(center.x, center.y + readoutR * 30 / 100), GColorBlack);

    /* 一刻の長さ */
    snprintf(buf, sizeof(buf), "一刻%d分", (int)(koku.length + 0.5f));
    drawCentered(ctx, buf, g.jp, GPoint(center.x, center.y + readoutR * 61 / 100), COLOR_SUBTEXT);

#if !defined(PBL_ROUND)
    /* --- 四隅：日付と、昼・夜それぞれの一刻 --- */
    int16_t lineH = graphics_text_layout_get_content_size(
        "昼", g.jp, GRect(0, 0, 100, 100), GTextOverflowModeFill, GTextAlignmentLeft).h + 4;
    int16_t pad = 3;
    GRect tl = GRect(bounds.origin.x + pad, bounds.origin.y, bounds.size.w / 2, lineH);
    GRect tr = GRect(bounds.origin.x + bounds.size.w / 2 - pad, bounds.origin.y,
                     bounds.size.w / 2, lineH);
    GRect bl = GRect(bounds.origin.x + pad, bounds.origin.y + bounds.size.h - lineH + 1,
                     bounds.size.w / 2, lineH);
    GRect br = GRect(bounds.origin.x + bounds.size.w / 2 - pad, bl.origin.y,
                     bounds.size.w / 2, lineH);

    int mon = g.now.tm_mon, mday = g.now.tm_mday;
#ifdef DEMO_YDAY
    static const int kMonthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    for (mon = 0, mday = DEMO_YDAY + 1; mday > kMonthDays[mon]; mday -= kMonthDays[mon++]) {}
#endif
    snprintf(buf, sizeof(buf), "%d/%d", mon + 1, mday);
    drawAligned(ctx, buf, g.jp, tl, GTextAlignmentLeft, COLOR_SKY_INK);
    drawAligned(ctx, kWeekdays[g.now.tm_wday % 7], g.jp, tr, GTextAlignmentRight, COLOR_SKY_INK);

    snprintf(buf, sizeof(buf), "昼%d分", (int)(koku.day_length + 0.5f));
    drawAligned(ctx, buf, g.jp, bl, GTextAlignmentLeft, COLOR_GROUND_INK);
    snprintf(buf, sizeof(buf), "夜%d分", (int)(koku.night_length + 0.5f));
    drawAligned(ctx, buf, g.jp, br, GTextAlignmentRight, COLOR_GROUND_INK);
#endif
}

// --------------------------------------------------------------------------
// イベント
// --------------------------------------------------------------------------

static void timeChanged(struct tm* tickTime, TimeUnits unitsChanged) {
    g.now = *tickTime;
    updateDay(false);
    layer_mark_dirty(g.layer);
}

static void messageReceived(DictionaryIterator* received, void* context) {
    bool changed = false;
    Tuple* t;

    if ((t = dict_find(received, MESSAGE_KEY_LATITUDE))) {
        g.latitude = t->value->int32;
        persist_write_int(PersistKeyLatitude, g.latitude);
        changed = true;
    }
    if ((t = dict_find(received, MESSAGE_KEY_LONGITUDE))) {
        g.longitude = t->value->int32;
        persist_write_int(PersistKeyLongitude, g.longitude);
        changed = true;
    }
    if ((t = dict_find(received, MESSAGE_KEY_TIMEZONE))) {
        g.timezone = t->value->int32;
        persist_write_int(PersistKeyTimezone, g.timezone);
        changed = true;
    }
    if ((t = dict_find(received, MESSAGE_KEY_BASIS))) {
        g.edo = t->value->int32 != 0;
        persist_write_bool(PersistKeyBasis, g.edo);
        changed = true;
    }

    if (changed) {
        updateDay(true);
        layer_mark_dirty(g.layer);
    }
}

// --------------------------------------------------------------------------
// 起動と終了
// --------------------------------------------------------------------------

static void init(void) {
    g.latitude = persist_exists(PersistKeyLatitude)
        ? persist_read_int(PersistKeyLatitude) : DEFAULT_LATITUDE;
    g.longitude = persist_exists(PersistKeyLongitude)
        ? persist_read_int(PersistKeyLongitude) : DEFAULT_LONGITUDE;
    g.edo = persist_exists(PersistKeyBasis) && persist_read_bool(PersistKeyBasis);
#ifdef DEMO_EDO
    g.edo = true;
#endif
    g.timezone = persist_exists(PersistKeyTimezone)
        ? persist_read_int(PersistKeyTimezone) : DEFAULT_TIMEZONE;

    g.jp = fonts_load_custom_font(resource_get_handle(FONT_JP));
    g.brush = fonts_load_custom_font(resource_get_handle(FONT_BRUSH));
    g.brushSmall = fonts_load_custom_font(resource_get_handle(FONT_BRUSH_SMALL));
    g.timeFont = fonts_get_system_font(FONT_KEY_TIME);

    g.window = window_create();
    window_set_background_color(g.window, GColorBlack);
    window_stack_push(g.window, true);

    Layer* root = window_get_root_layer(g.window);
    g.layer = layer_create(layer_get_bounds(root));
    layer_set_update_proc(g.layer, drawClock);
    layer_add_child(root, g.layer);

    time_t now = time(NULL);
    g.now = *localtime(&now);
    updateDay(true);

    app_message_register_inbox_received(messageReceived);
    app_message_open(128, 64);

    tick_timer_service_subscribe(MINUTE_UNIT, timeChanged);
}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    layer_destroy(g.layer);
    window_destroy(g.window);
    fonts_unload_custom_font(g.jp);
    fonts_unload_custom_font(g.brush);
    fonts_unload_custom_font(g.brushSmall);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
