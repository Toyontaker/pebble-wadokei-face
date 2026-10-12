/*
 * 日の出・日の入りの計算（NOAA の簡略式）。
 * 腕時計には libm が無いので、三角関数は Pebble の整数テーブル
 * （sin_lookup / cos_lookup / atan2_lookup）で代用する。精度は 1〜2 分。
 */

#include "sun.h"

#ifdef SUN_HOST_TEST
#include "test_shim.h"
#else
#include <pebble.h>
#endif

#define PI_F 3.14159265f

static inline int32_t rad_to_trig(float rad) {
    int32_t a = (int32_t)(rad * (TRIG_MAX_ANGLE / (2.0f * PI_F)));
    a %= TRIG_MAX_ANGLE;
    if (a < 0) a += TRIG_MAX_ANGLE;
    return a;
}

static inline float fsin(float rad) {
    return (float)sin_lookup(rad_to_trig(rad)) / TRIG_MAX_RATIO;
}

static inline float fcos(float rad) {
    return (float)cos_lookup(rad_to_trig(rad)) / TRIG_MAX_RATIO;
}

static float fsqrt(float x) {
    if (x <= 0) return 0;
    float r = x > 1 ? x : 1;
    for (int i = 0; i < 20; ++i) {
        r = 0.5f * (r + x / r);
    }
    return r;
}

/* acos を度で返す。x は [-1, 1] にクランプ済みであること。 */
static float facos_deg(float x) {
    float y = fsqrt(1.0f - x * x);
    int32_t a = atan2_lookup((int16_t)(y * 32000), (int16_t)(x * 32000));
    return (float)a * 360.0f / TRIG_MAX_ANGLE;
}

void sun_times(int yday, float lat, float lon, int32_t utc_offset_min,
               float depression, SunTimes* out) {
    const float deg = PI_F / 180.0f;

    /* 年周角（正午基準） */
    float g = 2.0f * PI_F / 365.0f * yday;

    float eqtime = 229.18f * (0.000075f
        + 0.001868f * fcos(g) - 0.032077f * fsin(g)
        - 0.014615f * fcos(2 * g) - 0.040849f * fsin(2 * g));

    float decl = 0.006918f
        - 0.399912f * fcos(g) + 0.070257f * fsin(g)
        - 0.006758f * fcos(2 * g) + 0.000907f * fsin(2 * g)
        - 0.002697f * fcos(3 * g) + 0.00148f * fsin(3 * g);

    float latr = lat * deg;
    float zenith = (90.0f + depression) * deg;
    float coslat = fcos(latr);
    float cosdecl = fcos(decl);
    float x = (fcos(zenith) - fsin(latr) * fsin(decl)) / (coslat * cosdecl);

    out->status = 0;
    if (x > 1.0f) {
        x = 1.0f;
        out->status = -1;
    } else if (x < -1.0f) {
        x = -1.0f;
        out->status = +1;
    }
    float ha = facos_deg(x);

    out->noon = 720.0f - 4.0f * lon - eqtime + utc_offset_min;
    out->rise = out->noon - 4.0f * ha;
    out->set  = out->noon + 4.0f * ha;
}
