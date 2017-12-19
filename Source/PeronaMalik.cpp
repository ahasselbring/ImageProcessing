/**
 * @file PeronaMalik.cpp
 *
 * This file implements the PeronaMalik class.
 *
 * @author Arne Hasselbring
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>

#include <x86intrin.h>

#include "Chronometer.h"
#include "Image.h"

#include "PeronaMalik.h"

Image PeronaMalik::apply(const Image& image, float kappa, float dt, unsigned int times, bool isotropic, OptimizationLevel optimizationLevel)
{
  switch(optimizationLevel)
  {
    case OptimizationLevel::sse4:
      if(isotropic)
        return applyT<true, true, false>(image, kappa, dt, times);
      else
        return applyT<false, true, false>(image, kappa, dt, times);
    case OptimizationLevel::avx2:
      if(isotropic)
        return applyT<true, true, true>(image, kappa, dt, times);
      else
        return applyT<false, true, true>(image, kappa, dt, times);
    case OptimizationLevel::noOptimization:
    default:
      if(isotropic)
        return applyT<true, false, false>(image, kappa, dt, times);
      else
        return applyT<false, false, false>(image, kappa, dt, times);
  }
}

template<bool isotropic>
__always_inline void PeronaMalik::diffusionSSE(__m128i firstDerivativeXi, __m128i firstDerivativeYi, __m128i& res, __m128 kappaSqrVec, __m128 dtVec, __m128& lastScaledFirstDerivativeX, float*& cacheptr)
{
  __m128 scaledFirstDerivativeX, scaledFirstDerivativeY;
  __m128 firstDerivativeX = _mm_cvtepi32_ps(firstDerivativeXi), firstDerivativeY = _mm_cvtepi32_ps(firstDerivativeYi);

  if(isotropic)
  {
    __m128 sqrNormXY = _mm_add_ps(_mm_mul_ps(firstDerivativeX, firstDerivativeX), _mm_mul_ps(firstDerivativeY, firstDerivativeY));
    __m128 g = _mm_div_ps(kappaSqrVec, _mm_add_ps(kappaSqrVec, sqrNormXY));
    scaledFirstDerivativeX = _mm_mul_ps(firstDerivativeX, g);
    scaledFirstDerivativeY = _mm_mul_ps(firstDerivativeY, g);
  }
  else
  {
    __m128 gX = _mm_div_ps(kappaSqrVec, _mm_add_ps(kappaSqrVec, _mm_mul_ps(firstDerivativeX, firstDerivativeX)));
    __m128 gY = _mm_div_ps(kappaSqrVec, _mm_add_ps(kappaSqrVec, _mm_mul_ps(firstDerivativeY, firstDerivativeY)));
    scaledFirstDerivativeX = _mm_mul_ps(firstDerivativeX, gX);
    scaledFirstDerivativeY = _mm_mul_ps(firstDerivativeY, gY);
  }

  __m128 lastColScaledFirstDerivativeX = _mm_castsi128_ps(_mm_alignr_epi8(_mm_castps_si128(scaledFirstDerivativeX), _mm_castps_si128(lastScaledFirstDerivativeX), 12));
  __m128 lastRowScaledFirstDerivativeY = _mm_load_ps(cacheptr);

  __m128 eulerStep = _mm_mul_ps(dtVec, _mm_add_ps(_mm_sub_ps(scaledFirstDerivativeX, lastColScaledFirstDerivativeX), _mm_sub_ps(scaledFirstDerivativeY, lastRowScaledFirstDerivativeY)));
  res = _mm_cvtps_epi32(eulerStep);

  lastScaledFirstDerivativeX = scaledFirstDerivativeX;
  _mm_store_ps(cacheptr, scaledFirstDerivativeY);

  cacheptr += 4;
}

template<bool isotropic>
__always_inline void PeronaMalik::diffusionAVX(__m256i firstDerivativeXi, __m256i firstDerivativeYi, __m256i& res, __m256 kappaSqrVec, __m256 dtVec, __m256& lastScaledFirstDerivativeX, float*& cacheptr)
{
  __m256 scaledFirstDerivativeX, scaledFirstDerivativeY;
  __m256 firstDerivativeX = _mm256_cvtepi32_ps(firstDerivativeXi), firstDerivativeY = _mm256_cvtepi32_ps(firstDerivativeYi);

  if(isotropic)
  {
    __m256 sqrNormXY = _mm256_add_ps(_mm256_mul_ps(firstDerivativeX, firstDerivativeX), _mm256_mul_ps(firstDerivativeY, firstDerivativeY));
    __m256 g = _mm256_div_ps(kappaSqrVec, _mm256_add_ps(kappaSqrVec, sqrNormXY));
    scaledFirstDerivativeX = _mm256_mul_ps(firstDerivativeX, g);
    scaledFirstDerivativeY = _mm256_mul_ps(firstDerivativeY, g);
  }
  else
  {
    __m256 gX = _mm256_div_ps(kappaSqrVec, _mm256_add_ps(kappaSqrVec, _mm256_mul_ps(firstDerivativeX, firstDerivativeX)));
    __m256 gY = _mm256_div_ps(kappaSqrVec, _mm256_add_ps(kappaSqrVec, _mm256_mul_ps(firstDerivativeY, firstDerivativeY)));
    scaledFirstDerivativeX = _mm256_mul_ps(firstDerivativeX, gX);
    scaledFirstDerivativeY = _mm256_mul_ps(firstDerivativeY, gY);
  }

  __m256 lastColScaledFirstDerivativeX = _mm256_castsi256_ps(_mm256_alignr_epi8(_mm256_castps_si256(scaledFirstDerivativeX), _mm256_castps_si256(_mm256_permute2f128_ps(lastScaledFirstDerivativeX, scaledFirstDerivativeX, (2 << 4) | 1)), 12));
  __m256 lastRowScaledFirstDerivativeY = _mm256_load_ps(cacheptr);

  __m256 eulerStep = _mm256_mul_ps(dtVec, _mm256_add_ps(_mm256_sub_ps(scaledFirstDerivativeX, lastColScaledFirstDerivativeX), _mm256_sub_ps(scaledFirstDerivativeY, lastRowScaledFirstDerivativeY)));
  res = _mm256_cvtps_epi32(eulerStep);

  lastScaledFirstDerivativeX = scaledFirstDerivativeX;
  _mm256_store_ps(cacheptr, scaledFirstDerivativeY);

  cacheptr += 8;
}

template<bool isotropic, bool simd, bool avx>
Image PeronaMalik::applyT(const Image& image, float kappa, float dt, unsigned int times)
{
  Chronometer time(simd ? (avx ? "PeronaMalik::applyT<?, true, true>" : "PeronaMalik::applyT<?, true, false>") : "PeronaMalik::applyT<?, false, false>");

  if(!image.aligned)
    throw std::runtime_error("Image must be aligned!");

  Image result1(image.width, image.height, true);
  Image result2(image.width, image.height, true);

  float* cache = nullptr;
  if(posix_memalign(reinterpret_cast<void**>(&cache), 32, image.width * sizeof(float)) != 0)
    throw std::runtime_error("Could not allocate aligned memory!");

  const float kappaSqr = kappa * kappa;

  const Image* src = &image;
  Image* dst = &result1;

  __m128 kappaSqrVecSSE, dtVecSSE;
  __m256 kappaSqrVecAVX, dtVecAVX;
  if(simd)
  {
    if(avx)
    {
      kappaSqrVecAVX = _mm256_set1_ps(kappaSqr);
      dtVecAVX = _mm256_set1_ps(dt);
    }
    else
    {
      kappaSqrVecSSE = _mm_set1_ps(kappaSqr);
      dtVecSSE = _mm_set1_ps(dt);
    }
  }

  int roundingMode = _MM_GET_ROUNDING_MODE();
  _MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);

  for(unsigned int i = 0; i < times; i++)
  {
    std::memset(cache, 0, image.width * sizeof(float));
    for(unsigned int y = 0; y < image.height; y++)
    {
      if(simd)
      {
        float* cacheptr = cache;
        if(avx)
        {
          const __m256i* srcRow = reinterpret_cast<const __m256i*>((*src)[y]);
          __m256i* dstRow = reinterpret_cast<__m256i*>((*dst)[y]);

          const __m256i* nextRow = reinterpret_cast<const __m256i*>((*src)[y + 1]);
          const __m256i* const srcRowEnd = nextRow;

          __m256i src = _mm256_load_si256(srcRow);
          __m256i nextSrc;
          __m256 lastScaledFirstDerivativeX = _mm256_setzero_ps();
          while(srcRow != srcRowEnd)
          {
            nextSrc = (++srcRow != srcRowEnd) ? _mm256_load_si256(srcRow) : _mm256_setzero_si256();
            __m256i row = src;
            __m256i rowy = (y != image.height - 1) ? _mm256_load_si256(nextRow++) : _mm256_setzero_si256();
            __m256i rowx = _mm256_alignr_epi8(_mm256_permute2x128_si256(src, nextSrc, (2 << 4) | 1), src, 1);
            src = nextSrc;

            __m256i row16a = _mm256_unpacklo_epi8(row, _mm256_setzero_si256());
            __m256i row16b = _mm256_unpackhi_epi8(row, _mm256_setzero_si256());
            __m256i rowx16a = _mm256_unpacklo_epi8(rowx, _mm256_setzero_si256());
            __m256i rowx16b = _mm256_unpackhi_epi8(rowx, _mm256_setzero_si256());
            __m256i firstDerivativeXa = _mm256_sub_epi16(rowx16a, row16a);
            __m256i firstDerivativeXb = _mm256_sub_epi16(rowx16b, row16b);
            __m256i rowy16a = _mm256_unpacklo_epi8(rowy, _mm256_setzero_si256());
            __m256i rowy16b = _mm256_unpackhi_epi8(rowy, _mm256_setzero_si256());
            __m256i firstDerivativeYa = _mm256_sub_epi16(rowy16a, row16a);
            __m256i firstDerivativeYb = _mm256_sub_epi16(rowy16b, row16b);

            __m256i lo, hi, tmp;

            diffusionAVX<isotropic>(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeXa, 0)), _mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeYa, 0)), lo, kappaSqrVecAVX, dtVecAVX, lastScaledFirstDerivativeX, cacheptr);
            diffusionAVX<isotropic>(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeXb, 0)), _mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeYb, 0)), hi, kappaSqrVecAVX, dtVecAVX, lastScaledFirstDerivativeX, cacheptr);
            diffusionAVX<isotropic>(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeXa, 1)), _mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeYa, 1)), tmp, kappaSqrVecAVX, dtVecAVX, lastScaledFirstDerivativeX, cacheptr);
            lo = _mm256_packs_epi32(_mm256_permute2x128_si256(lo, tmp, (2 << 4) | 0), _mm256_permute2x128_si256(lo, tmp, (3 << 4) | 1));
            diffusionAVX<isotropic>(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeXb, 1)), _mm256_cvtepi16_epi32(_mm256_extracti128_si256(firstDerivativeYb, 1)), tmp, kappaSqrVecAVX, dtVecAVX, lastScaledFirstDerivativeX, cacheptr);
            hi = _mm256_packs_epi32(_mm256_permute2x128_si256(hi, tmp, (2 << 4) | 0), _mm256_permute2x128_si256(hi, tmp, (3 << 4) | 1));

            lo = _mm256_add_epi16(lo, row16a);
            hi = _mm256_add_epi16(hi, row16b);
            __m256i result = _mm256_packus_epi16(lo, hi);
            _mm256_stream_si256(dstRow++, result);
          }
        }
        else
        {
          const __m128i* srcRow = reinterpret_cast<const __m128i*>((*src)[y]);
          __m128i* dstRow = reinterpret_cast<__m128i*>((*dst)[y]);

          const __m128i* nextRow = reinterpret_cast<const __m128i*>((*src)[y + 1]);
          const __m128i* const srcRowEnd = nextRow;

          __m128i src = _mm_load_si128(srcRow);
          __m128i nextSrc;
          __m128 lastScaledFirstDerivativeX = _mm_setzero_ps();
          while(srcRow != srcRowEnd)
          {
            nextSrc = (++srcRow != srcRowEnd) ? _mm_load_si128(srcRow) : _mm_setzero_si128();
            __m128i row = src;
            __m128i rowy = (y != image.height - 1) ? _mm_load_si128(nextRow++) : _mm_setzero_si128();
            __m128i rowx = _mm_alignr_epi8(nextSrc, row, 1);
            src = nextSrc;

            __m128i row16a = _mm_unpacklo_epi8(row, _mm_setzero_si128());
            __m128i row16b = _mm_unpackhi_epi8(row, _mm_setzero_si128());
            __m128i rowx16a = _mm_unpacklo_epi8(rowx, _mm_setzero_si128());
            __m128i rowx16b = _mm_unpackhi_epi8(rowx, _mm_setzero_si128());
            __m128i firstDerivativeXa = _mm_sub_epi16(rowx16a, row16a);
            __m128i firstDerivativeXb = _mm_sub_epi16(rowx16b, row16b);
            __m128i rowy16a = _mm_unpacklo_epi8(rowy, _mm_setzero_si128());
            __m128i rowy16b = _mm_unpackhi_epi8(rowy, _mm_setzero_si128());
            __m128i firstDerivativeYa = _mm_sub_epi16(rowy16a, row16a);
            __m128i firstDerivativeYb = _mm_sub_epi16(rowy16b, row16b);

            __m128i lo, hi, tmp;

            diffusionSSE<isotropic>(_mm_cvtepi16_epi32(firstDerivativeXa), _mm_cvtepi16_epi32(firstDerivativeYa), lo, kappaSqrVecSSE, dtVecSSE, lastScaledFirstDerivativeX, cacheptr);
            diffusionSSE<isotropic>(_mm_cvtepi16_epi32(_mm_srli_si128(firstDerivativeXa, 8)), _mm_cvtepi16_epi32(_mm_srli_si128(firstDerivativeYa, 8)), hi, kappaSqrVecSSE, dtVecSSE, lastScaledFirstDerivativeX, cacheptr);
            lo = _mm_packs_epi32(lo, hi);

            diffusionSSE<isotropic>(_mm_cvtepi16_epi32(firstDerivativeXb), _mm_cvtepi16_epi32(firstDerivativeYb), hi, kappaSqrVecSSE, dtVecSSE, lastScaledFirstDerivativeX, cacheptr);
            diffusionSSE<isotropic>(_mm_cvtepi16_epi32(_mm_srli_si128(firstDerivativeXb, 8)), _mm_cvtepi16_epi32(_mm_srli_si128(firstDerivativeYb, 8)), tmp, kappaSqrVecSSE, dtVecSSE, lastScaledFirstDerivativeX, cacheptr);
            hi = _mm_packs_epi32(hi, tmp);

            lo = _mm_add_epi16(lo, row16a);
            hi = _mm_add_epi16(hi, row16b);
            __m128i result = _mm_packus_epi16(lo, hi);
            _mm_stream_si128(dstRow++, result);
          }
        }
      }
      else
      {
        float lastScaledFirstDerivativeX = 0.f;
        const std::uint8_t* srcRow = (*src)[y];
        const std::uint8_t* nextRow = (*src)[y + 1];
        std::uint8_t* dstRow = (*dst)[y];
        for(unsigned int x = 0; x < image.width; x++)
        {
          float firstDerivativeX = (x != image.width - 1 ? srcRow[x + 1] : 0) - srcRow[x];
          float firstDerivativeY = (y != image.height - 1 ? nextRow[x] : 0) - srcRow[x];

          float scaledFirstDerivativeX, scaledFirstDerivativeY;

          if(isotropic)
          {
            float g = kappaSqr / (kappaSqr + firstDerivativeX * firstDerivativeX + firstDerivativeY * firstDerivativeY);
            scaledFirstDerivativeX = firstDerivativeX * g;
            scaledFirstDerivativeY = firstDerivativeY * g;
          }
          else
          {
            float gX = kappaSqr / (kappaSqr + firstDerivativeX * firstDerivativeX);
            float gY = kappaSqr / (kappaSqr + firstDerivativeY * firstDerivativeY);
            scaledFirstDerivativeX = firstDerivativeX * gX;
            scaledFirstDerivativeY = firstDerivativeY * gY;
          }

          float eulerStep = dt * ((scaledFirstDerivativeX - lastScaledFirstDerivativeX) + (scaledFirstDerivativeY - cache[x]));
          std::int32_t offset = static_cast<std::int32_t>(eulerStep);

          if(offset < std::numeric_limits<std::int16_t>::min())
            offset = std::numeric_limits<std::int16_t>::min();
          else if(offset > std::numeric_limits<std::int16_t>::max())
            offset = std::numeric_limits<std::int16_t>::max();

          std::int16_t newVal = static_cast<std::int16_t>(srcRow[x]) + static_cast<std::int16_t>(offset);
          if(newVal < 0)
            newVal = 0;
          else if(newVal > 255)
            newVal = 255;
          dstRow[x] = newVal;

          lastScaledFirstDerivativeX = scaledFirstDerivativeX;
          cache[x] = scaledFirstDerivativeY;
        }
      }
    }

    src = dst;
    dst = (dst == &result1 ? &result2 : &result1);
  }

  _MM_SET_ROUNDING_MODE(roundingMode);

  free(cache);

  return *src;
}
