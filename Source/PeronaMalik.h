/**
 * @file PeronaMalik.h
 *
 * This file declares the PeronaMalik class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include "Operator.h"
#include "OptimizationLevel.h"
#include "SIMD.h"

class Image;

/**
 * @brief This class implements the Perona Malik diffusion denoising filter.
 */
class PeronaMalik : public Operator
{
public:
  /*
   * @brief Constructs a filter.
   * @param kappa The larger, the less the diffusion is blocked at edges.
   * @param dt The step length in the numeric solution of the diffusion equation.
   * @param times The number of iterations (i.e. the solution is evaluated at dt*times).
   * @param isotropic Whether isotropic (true) or anisotropic (false) diffusion tensors should be used.
   * @param optimizationLevel The kind of optimization that should be used.
   */
  PeronaMalik(float kappa, float dt, unsigned int times, bool isotropic = false, OptimizationLevel = OptimizationLevel::noOptimization);
  /**
   * @brief Denoises an image.
   * @param image The image that is denoised.
   * @return A denoised image.
   */
  Image apply(const Image& image) override;
private:
  /**
   * @brief Computes the increment (Euler step) to the image.
   * @tparam isotropic Whether isotropic (true) or anisotropic (false) diffusion tensors should be used.
   * @param firstDerivativeXi The first derivatives in x direction (as packed 32-bit integers).
   * @param firstDerivativeYi The first derivatives in y direction (as packed 32-bit integers).
   * @param res The step that has to be added to the image.
   * @param kappaSqrVec A single precision vector containing kappa squared.
   * @param dtVec A single precision vector containing the step length.
   * @param lastScaledFirstDerivativeX The value of the scaled first derivative in x direction of the previous column.
   * @param cacheptr A pointer that points to the scaled first derivatives in y direction of the previous row.
   */
  template<bool isotropic>
  static void diffusionSSE(__m128i firstDerivativeXi, __m128i firstDerivativeYi, __m128i& res, __m128 kappaSqrVec, __m128 dtVec, __m128& lastScaledFirstDerivativeX, float*& cacheptr);
  /**
   * @brief Computes the increment (Euler step) to the image.
   * @tparam isotropic Whether isotropic (true) or anisotropic (false) diffusion tensors should be used.
   * @param firstDerivativeXi The first derivatives in x direction (as packed 32-bit integers).
   * @param firstDerivativeYi The first derivatives in y direction (as packed 32-bit integers).
   * @param res The step that has to be added to the image.
   * @param kappaSqrVec A single precision vector containing kappa squared.
   * @param dtVec A single precision vector containing the step length.
   * @param lastScaledFirstDerivativeX The value of the scaled first derivative in x direction of the previous column.
   * @param cacheptr A pointer that points to the scaled first derivatives in y direction of the previous row.
   */
  template<bool isotropic>
  static void diffusionAVX(__m256i firstDerivativeXi, __m256i firstDerivativeYi, __m256i& res, __m256 kappaSqrVec, __m256 dtVec, __m256& lastScaledFirstDerivativeX, float*& cacheptr);
  /**
   * @brief Denoises an image.
   * @tparam isotropic Whether isotropic (true) or anisotropic (false) diffusion tensors should be used.
   * @tparam simd Whether SIMD instructions should be used.
   * @tparam avx Whether AVX instructions should be used (instead of SSE instructions).
   * @param image The image that is denoised.
   * @param kappa The larger, the less the diffusion is blocked at edges.
   * @param dt The step length in the numeric solution of the diffusion equation.
   * @param times The number of iterations (i.e. the solution is evaluated at dt*times).
   * @return A denoised image.
   */
  template<bool isotropic, bool simd, bool avx>
  static Image applyT(const Image& image, float kappa, float dt, unsigned int times);
  float kappa;                         ///< The larger, the less the diffusion is blocked at edges.
  float dt;                            ///< The step length in the numeric solution of the diffusion equation.
  unsigned int times;                  ///< The number of iterations (i.e. the solution is evaluated at dt*times).
  bool isotropic;                      ///< Whether isotropic (true) or anisotropic (false) diffusion tensors should be used.
  OptimizationLevel optimizationLevel; ///< The kind of optimization that should be used.
};
