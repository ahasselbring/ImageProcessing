/**
 * @file Avg5.cpp
 *
 * This file implements the Avg5 class.
 *
 * @author Arne Hasselbring
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <x86intrin.h>

#include "Chronometer.h"
#include "Image.h"

#include "Avg5.h"

Image Avg5::apply(const Image& image, OptimizationLevel optimizationLevel)
{
  switch (optimizationLevel)
  {
    case OptimizationLevel::sse4:
      return applyT<true, false>(image);
    case OptimizationLevel::avx2:
      return applyT<true, true>(image);
    case OptimizationLevel::noOptimization:
    default:
      return applyT<false, false>(image);
  }
}

template<bool simd, bool avx>
Image Avg5::applyT(const Image& image)
{
  Chronometer time(simd ? (avx ? "Avg5::applyT<true, true>" : "Avg5::applyT<true, false>") : "Avg5::applyT<false, false>");

  if(!image.aligned)
    throw std::runtime_error("Image must be aligned!");

  Image result(image.width, image.height, true);

  std::uint8_t* zerow = nullptr;
  if(posix_memalign(reinterpret_cast<void**>(&zerow), 32, image.width) != 0)
    throw std::runtime_error("Could not allocate aligned memory!");
  std::memset(zerow, 0, image.width);

  if(simd)
  {
    if(avx)
    {
      __m256i factor = _mm256_set1_epi16((1 << 16) / 5 + 2);

      const __m256i* srcRow =  reinterpret_cast<const __m256i*>(image[0]);
      __m256i* dstRow = reinterpret_cast<__m256i*>(result[0]);

      for(unsigned int y = 0; y < image.height; y++)
      {
        const __m256i* prevRow = reinterpret_cast<const __m256i*>(y > 0 ? image[y - 1] : zerow);
        const __m256i* const srcRowEnd = reinterpret_cast<const __m256i*>(image[y + 1]);
        const __m256i* nextRow = reinterpret_cast<const __m256i*>(y < image.height - 1 ? image[y + 1] : zerow);

        __m256i src = _mm256_load_si256(srcRow);
        __m256i nextSrc;
        __m256i lastSrc = _mm256_setzero_si256();
        while(srcRow != srcRowEnd)
        {
          nextSrc = (++srcRow != srcRowEnd) ? _mm256_load_si256(srcRow) : _mm256_setzero_si256();
          __m256i above = _mm256_load_si256(prevRow++);
          __m256i mid = src;
          __m256i below = _mm256_load_si256(nextRow++);
          __m256i left = _mm256_alignr_epi8(src, _mm256_permute2x128_si256(src, lastSrc, (0 << 4) | 3), 15);
          __m256i right = _mm256_alignr_epi8(_mm256_permute2x128_si256(src, nextSrc, (2 << 4) | 1), src, 1);
          lastSrc = src;
          src = nextSrc;

          // This is actually a different order than one might expect due to unpack not being per 128-bit lane.
          // This effect is however reversed by the fact that pack works in same way.
          __m256i above1 = _mm256_unpacklo_epi8(above, _mm256_setzero_si256());
          __m256i mid1 = _mm256_unpacklo_epi8(mid, _mm256_setzero_si256());
          __m256i below1 = _mm256_unpacklo_epi8(below, _mm256_setzero_si256());
          __m256i left1 = _mm256_unpacklo_epi8(left, _mm256_setzero_si256());
          __m256i right1 = _mm256_unpacklo_epi8(right, _mm256_setzero_si256());

          __m256i result1 = _mm256_add_epi16(_mm256_add_epi16(_mm256_add_epi16(above1, below1), _mm256_add_epi16(left1, right1)), mid1);

          __m256i above2 = _mm256_unpackhi_epi8(above, _mm256_setzero_si256());
          __m256i mid2 = _mm256_unpackhi_epi8(mid, _mm256_setzero_si256());
          __m256i below2 = _mm256_unpackhi_epi8(below, _mm256_setzero_si256());
          __m256i left2 = _mm256_unpackhi_epi8(left, _mm256_setzero_si256());
          __m256i right2 = _mm256_unpackhi_epi8(right, _mm256_setzero_si256());

          __m256i result2 = _mm256_add_epi16(_mm256_add_epi16(_mm256_add_epi16(above2, below2), _mm256_add_epi16(left2, right2)), mid2);

          __m256i result = _mm256_packus_epi16(_mm256_mulhi_epu16(result1, factor), _mm256_mulhi_epu16(result2, factor));
          _mm256_stream_si256(dstRow, result);
          dstRow++;
        }
      }
    }
    else
    {
      __m128i factor = _mm_set1_epi16((1 << 16) / 5 + 2);

      const __m128i* srcRow =  reinterpret_cast<const __m128i*>(image[0]);
      __m128i* dstRow = reinterpret_cast<__m128i*>(result[0]);

      for(unsigned int y = 0; y < image.height; y++)
      {
        const __m128i* prevRow = reinterpret_cast<const __m128i*>(y > 0 ? image[y - 1] : zerow);
        const __m128i* const srcRowEnd =  reinterpret_cast<const __m128i*>(image[y] + image.width);
        const __m128i* nextRow = reinterpret_cast<const __m128i*>(y < image.height - 1 ? image[y + 1] : zerow);

        __m128i src = _mm_load_si128(srcRow);
        __m128i nextSrc;
        __m128i lastSrc = _mm_setzero_si128();
        while(srcRow != srcRowEnd)
        {
          nextSrc = (++srcRow != srcRowEnd) ? _mm_load_si128(srcRow) : _mm_setzero_si128();
          __m128i above = _mm_load_si128(prevRow++);
          __m128i mid = src;
          __m128i below = _mm_load_si128(nextRow++);
          __m128i left = _mm_alignr_epi8(src, lastSrc, 15);
          __m128i right = _mm_alignr_epi8(nextSrc, src, 1);
          lastSrc = src;
          src = nextSrc;

          __m128i above1 = _mm_unpacklo_epi8(above, _mm_setzero_si128());
          __m128i mid1 = _mm_unpacklo_epi8(mid, _mm_setzero_si128());
          __m128i below1 = _mm_unpacklo_epi8(below, _mm_setzero_si128());
          __m128i left1 = _mm_unpacklo_epi8(left, _mm_setzero_si128());
          __m128i right1 = _mm_unpacklo_epi8(right, _mm_setzero_si128());

          __m128i result1 = _mm_add_epi16(_mm_add_epi16(_mm_add_epi16(above1, below1), _mm_add_epi16(left1, right1)), mid1);

          __m128i above2 = _mm_unpackhi_epi8(above, _mm_setzero_si128());
          __m128i mid2 = _mm_unpackhi_epi8(mid, _mm_setzero_si128());
          __m128i below2 = _mm_unpackhi_epi8(below, _mm_setzero_si128());
          __m128i left2 = _mm_unpackhi_epi8(left, _mm_setzero_si128());
          __m128i right2 = _mm_unpackhi_epi8(right, _mm_setzero_si128());

          __m128i result2 = _mm_add_epi16(_mm_add_epi16(_mm_add_epi16(above2, below2), _mm_add_epi16(left2, right2)), mid2);

          __m128i result = _mm_packus_epi16(_mm_mulhi_epu16(result1, factor), _mm_mulhi_epu16(result2, factor));
          _mm_stream_si128(dstRow, result);
          dstRow++;
        }
      }
    }
  }
  else
  {
    for(unsigned int y = 0; y < image.height; y++)
    {
      const std::uint8_t* prevRow = y > 0 ? image[y - 1] : zerow;
      const std::uint8_t* srcRow =  image[y];
      const std::uint8_t* nextRow = y < image.height - 1 ? image[y + 1] : zerow;
      std::uint8_t* dstRow = result[y];

      dstRow[0] = (srcRow[0] + srcRow[1] + prevRow[0] + nextRow[0]) / 5;
      for(unsigned int x = 1; x < image.width - 1; x++)
      {
        dstRow[x] = (srcRow[x - 1] + srcRow[x] + srcRow[x + 1] + prevRow[x] + nextRow[x]) / 5;
      }
      dstRow[image.width - 1] = (srcRow[image.width - 2] + srcRow[image.width - 1] + prevRow[image.width - 1] + nextRow[image.width - 1]) / 5;
    }
  }

  free(zerow);

  return result;
}
