#pragma once
#include <stdbool.h>
#include <stdint.h>

/* 太陽の伏角（度）。日の出・日の入りは大気差と視半径を含めた 0.833°。
   江戸の寛政暦以降は、太陽が地平線下 7°21′40″ にある時を明け六つ・暮れ六つとした。 */
#define SUN_DEPRESSION_HORIZON 0.833f
#define SUN_DEPRESSION_EDO     7.3611f

typedef struct {
    float rise;   /* その日の地方時 0:00 からの分。前日・翌日にはみ出すこともある */
    float set;
    float noon;   /* 南中 */
    int8_t status; /* 0: 通常、+1: 一日中その高度より上、-1: 一日中下 */
} SunTimes;

/* yday は 1 月 1 日を 0 とする通日、lat/lon は度（東経・北緯が正）、
   utc_offset_min は UTC からの時差（分）。 */
void sun_times(int yday, float lat, float lon, int32_t utc_offset_min,
               float depression, SunTimes* out);
