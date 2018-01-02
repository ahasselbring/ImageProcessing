/**
 * @file SIMD.h
 *
 * This file includes platform specific headers for SIMD intrinsics.
 *
 * @author Arne Hasselbring
 */

#pragma once

#ifdef _WIN32
#include <intrin.h>

#define ALWAYSINLINE __forceinline
#else
#include <x86intrin.h>

#define ALWAYSINLINE inline __attribute__((always_inline))
#endif
