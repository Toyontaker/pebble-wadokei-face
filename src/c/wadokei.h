#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "sun.h"

/* 刻の番号。明け六つを 0 とし、昼の六刻、夜の六刻の順に 11 まで。 */
#define KOKU_COUNT 12

typedef struct {
    int8_t index;     /* 0..11 */
    bool half;        /* 刻の後半（「〜半」） */
    int8_t quarter;   /* 刻を四つに分けたうちの何番目か（0..3、「一つ」〜「四つ」） */
    float start;      /* この刻の始まり（今日の 0:00 からの分） */
    float length;     /* 一刻の長さ（分） */
    float day_length; /* 昼の一刻 */
    float night_length; /* 夜の一刻 */
} Koku;

/* 文字盤の一日。今日の六つと日の出・日の入り（今日の 0:00 からの分）。 */
typedef struct {
    float ake;     /* 明け六つ */
    float kure;    /* 暮れ六つ */
    float sunrise;
    float sunset;
    float noon;
    float ake_next;  /* 翌日の明け六つ（+1440 済み） */
    float kure_prev; /* 前日の暮れ六つ（-1440 済み） */
} WaDay;

void wadokei_day(int yday, float lat, float lon, int32_t utc_offset_min,
                 bool edo_twilight, WaDay* out);

/* now は今日の 0:00 からの分 */
void wadokei_koku(const WaDay* day, float now, Koku* out);

/* 刻の境目の時刻（今日の 0:00 からの分）。index は 0..11、夜は暮れ六つ起点。 */
float wadokei_boundary(const WaDay* day, int index);

const char* wadokei_name(int index);   /* 明け六つ など */
const char* wadokei_eto(int index);    /* 卯 など */
const char* wadokei_numeral(int index); /* 六 など */
const char* wadokei_quarter(int quarter); /* 一つ〜四つ（丑三つ の「三つ」） */
