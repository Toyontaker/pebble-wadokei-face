#pragma once
#include <math.h>
#include <stdint.h>
#define TRIG_MAX_ANGLE 0x10000
#define TRIG_MAX_RATIO 0xffff
static inline int32_t sin_lookup(int32_t a){ return (int32_t)lround(sin(a*2*M_PI/TRIG_MAX_ANGLE)*TRIG_MAX_RATIO); }
static inline int32_t cos_lookup(int32_t a){ return (int32_t)lround(cos(a*2*M_PI/TRIG_MAX_ANGLE)*TRIG_MAX_RATIO); }
static inline int32_t atan2_lookup(int16_t y,int16_t x){ double r=atan2(y,x); if(r<0) r+=2*M_PI; return (int32_t)(r*TRIG_MAX_ANGLE/(2*M_PI)); }
