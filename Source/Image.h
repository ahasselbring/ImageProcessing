/**
 * @file Image.hpp
 *
 * This file declares the Image class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

/**
 * @brief This class is a container for grayscale / single channel images.
 */
class Image final
{
public:
  /**
   * @brief Creates an Image and allocates memory.
   * @param width The width of the image (i.e. number of columns).
   * @param height The height of the image (i.e. number of rows).
   * @param aligned Whether the memory must be aligned for SSE/AVX.
   */
  Image(unsigned int width, unsigned int height, bool aligned = false) :
    width(width),
    height(height),
    aligned(aligned),
    data(nullptr)
  {
    if(aligned && (width % 32) != 0)
      throw std::runtime_error("The given width makes it impossible for this image to be properly aligned!");
    if(posix_memalign(reinterpret_cast<void**>(&data), 32, width * height) != 0)
      throw std::runtime_error("Could not allocate aligned memory!");
  }
  /**
   * @brief Copies an Image.
   * @param other The image that is copied into the new one.
   */
  Image(const Image& other) :
    width(other.width),
    height(other.height),
    aligned(other.aligned),
    data(nullptr)
  {
    if(posix_memalign(reinterpret_cast<void**>(&data), 32, width * height) != 0)
      throw std::runtime_error("Could not allocate aligned memory!");

    std::memcpy(data, other.data, width * height);
  }
  /**
   * @brief Frees the pixel memory.
   */
  ~Image()
  {
    if(data != nullptr)
      free(data);
  }
  /**
   * @brief Accesses a pixel row (mutable).
   * @param y The row (zero-based) that should be accessed.
   * @return A pointer to the start of the row.
   */
  std::uint8_t* operator[](unsigned int y)
  {
    return data + y * width;
  }
  /**
   * @brief Accesses a pixel row (read-only).
   * @param y The row (zero-based) that should be accessed.
   * @return A pointer to the start of the row.
   */
  const std::uint8_t* operator[](unsigned int y) const
  {
    return data + y * width;
  }
  unsigned int width;  ///< The width of the image (in pixels).
  unsigned int height; ///< The height of the image (in pixels).
  const bool aligned;  ///< Whether all the rows are aligned so that SSE/AVX instructions can be used.
private:
  std::uint8_t* data; ///< The pixel data (row by row).
};
