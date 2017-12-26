/**
 * @file ImageTools.h
 *
 * This file declares the ImageTools class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <string>

#include "Image.h"

/**
 * @brief This enum enumerates the image formats that can be handled by the ImageTools class.
 */
enum class ImageFormat
{
  PNG,              ///< The three channels correspond to red, green and blue, because the PNG contains the correct color information.
  YCbCr,            ///< The three channels correspond to Y, Cb and Cr.
  numOfImageFormats ///< The number of image formats.
};

/**
 * @brief This class provides helper functions to handle Images.
 */
class ImageTools
{
public:
  /**
   * @brief Loads an Image from a file.
   * @param path The path from which the Image should be loaded.
   * @param format The format in which the Image is stored.
   * @return The loaded Image.
   */
  static Image loadImage(const std::string& path, ImageFormat format);
  /*+
   * @brief Stores an Image to a file.
   * @param path The path where the Image should be stored.
   * @param image The Image to store.
   */
  static void storeImage(const std::string& path, const Image& image);
  /**
   * @brief Compares to Images for equality.
   * @param image1 The first operand.
   * @param image2 The second operand.
   * @return Whether the two Images are equal.
   */
  static bool compare(const Image& image1, const Image& image2);
private:
  /**
   * @brief Clamps an integer to the range of an unsigned char.
   * @param value An integer.
   * @return The clamped value.
   */
  static unsigned char clamp(int value);
};
