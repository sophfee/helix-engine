#pragma once

#include <cmath>
#include <cstdint>

typedef int8_t    s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float_t  f32;
typedef double_t f64;

#ifdef PACK_TARGET_EXPORT
#define API extern "C" __declspec(dllexport)
#elifdef PACK_TARGET_IMPORT
#define API extern "C" __declspec(dllimport)
#else
// If we aren't importing or exporting we don't need to make our functions extern "C"
#define API 
#endif