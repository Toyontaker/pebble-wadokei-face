/*
 * 不定時法。明け六つから暮れ六つまでの昼と、暮れ六つから翌朝の明け六つまでの夜を
 * それぞれ六等分する。数え方は九つ（正午・正子）から八つ、七つ、六つ、五つ、四つと減り、
 * また九つに戻る。
 */

#include "wadokei.h"

static const char* const kNames[KOKU_COUNT] = {
    "明け六つ", "朝五つ", "昼四つ", "昼九つ", "昼八つ", "夕七つ",
    "暮れ六つ", "宵五つ", "夜四つ", "夜九つ", "夜八つ", "暁七つ",
};

static const char* const kEto[KOKU_COUNT] = {
    "卯", "辰", "巳", "午", "未", "申",
    "酉", "戌", "亥", "子", "丑", "寅",
};

static const char* const kNumerals[KOKU_COUNT] = {
    "六", "五", "四", "九", "八", "七",
    "六", "五", "四", "九", "八", "七",
};

const char* wadokei_name(int index) { return kNames[index % KOKU_COUNT]; }
const char* wadokei_eto(int index) { return kEto[index % KOKU_COUNT]; }
const char* wadokei_numeral(int index) { return kNumerals[index % KOKU_COUNT]; }

static const char* const kQuarters[4] = { "一つ", "二つ", "三つ", "四つ" };

const char* wadokei_quarter(int quarter) { return kQuarters[quarter & 3]; }

/* 白夜・極夜で昼か夜が消えると割り算できないので、一刻を最低 1 分は残す。 */
static void clamp_span(float* a, float* b) {
    if (*b - *a < 6.0f) {
        float mid = (*a + *b) / 2;
        *a = mid - 3.0f;
        *b = mid + 3.0f;
    }
    if (*b - *a > 1440.0f - 6.0f) {
        float mid = (*a + *b) / 2;
        *a = mid - 717.0f;
        *b = mid + 717.0f;
    }
}

void wadokei_day(int yday, float lat, float lon, int32_t utc_offset_min,
                 bool edo_twilight, WaDay* out) {
    float dep = edo_twilight ? SUN_DEPRESSION_EDO : SUN_DEPRESSION_HORIZON;
    SunTimes t, prev, next;

    sun_times(yday, lat, lon, utc_offset_min, dep, &t);
    sun_times(yday - 1, lat, lon, utc_offset_min, dep, &prev);
    sun_times(yday + 1, lat, lon, utc_offset_min, dep, &next);
    clamp_span(&t.rise, &t.set);
    clamp_span(&prev.rise, &prev.set);
    clamp_span(&next.rise, &next.set);

    out->ake = t.rise;
    out->kure = t.set;
    out->noon = t.noon;
    out->kure_prev = prev.set - 1440.0f;
    out->ake_next = next.rise + 1440.0f;

    if (edo_twilight) {
        SunTimes h;
        sun_times(yday, lat, lon, utc_offset_min, SUN_DEPRESSION_HORIZON, &h);
        out->sunrise = h.rise;
        out->sunset = h.set;
    } else {
        out->sunrise = t.rise;
        out->sunset = t.set;
    }
}

float wadokei_boundary(const WaDay* day, int index) {
    float daylen = (day->kure - day->ake) / 6.0f;
    float nightlen = (1440.0f - (day->kure - day->ake)) / 6.0f;
    if (index < 6) {
        return day->ake + daylen * index;
    }
    return day->kure + nightlen * (index - 6);
}

void wadokei_koku(const WaDay* day, float now, Koku* out) {
    float start, end;
    int base;

    if (now < day->ake) {
        start = day->kure_prev;
        end = day->ake;
        base = 6;
    } else if (now < day->kure) {
        start = day->ake;
        end = day->kure;
        base = 0;
    } else {
        start = day->kure;
        end = day->ake_next;
        base = 6;
    }

    float len = (end - start) / 6.0f;
    int k = (int)((now - start) / len);
    if (k < 0) k = 0;
    if (k > 5) k = 5;

    out->index = base + k;
    out->start = start + len * k;
    out->length = len;
    out->half = (now - out->start) >= len / 2;
    int q = (int)((now - out->start) * 4 / len);
    out->quarter = q < 0 ? 0 : q > 3 ? 3 : q;
    out->day_length = (day->kure - day->ake) / 6.0f;
    out->night_length = base == 6 ? len : (day->ake_next - day->kure) / 6.0f;
}
