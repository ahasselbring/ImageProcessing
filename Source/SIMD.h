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
#else
#include <x86intrin.h>
#endif
