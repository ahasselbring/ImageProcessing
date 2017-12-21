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
 * @brief This class provides helper functions to handle Images.
 */
class ImageTools
{
public:
  /**
   * @brief Loads an Image from a file.
   * @param path The path from which the Image should be loaded.
   * @return The loaded Image.
   */
  static Image loadImage(const std::string& path);
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
};
