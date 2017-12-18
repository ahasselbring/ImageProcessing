/**
 * @file OptimizationLevel
 *
 * This file declares the OptimizationLevel enum.
 *
 * @author Arne Hasselbring
 */

#pragma once

/**
 * @brief This enum enumerates the different levels of optimization of image processing algorithms.
 */
enum class OptimizationLevel
{
  noOptimization,         ///< The code is written in plain C++ (compiler optimizations are not influenced by this).
  sse4,                   ///< Intrinsics for SSE up to SSE4.1 are used.
  avx2,                   ///< Intrinsics for AVX2 are used.
  numOfOptimizationLevels ///< The number of optimization levels.
};
