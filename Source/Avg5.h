/**
 * @file Avg5.h
 *
 * This file declares the Avg5 class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include "OptimizationLevel.h"

class Image;

/**
 * @brief This class implements a denoising filter that takes the average of a pixel and its four neighbors.
 */
class Avg5 final
{
public:
  /**
   * @brief Denoises an image.
   * @param image The image that is denoised.
   * @param optimizationLevel The kind of optimization that should be used.
   * @return A denoised image.
   */
  static Image apply(const Image& image, OptimizationLevel optimizationLevel = OptimizationLevel::noOptimization);
private:
  /**
   * @brief Denoises an image.
   * @tparam simd Whether SIMD instructions should be used.
   * @tparam avx Whether AVX instructions should be used (instead of SSE instructions).
   * @param image The image that is denoised.
   * @return A denoised image.
   */
  template<bool simd, bool avx>
  static Image applyT(const Image& image);
};
