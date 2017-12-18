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
 * @brief This class provides functions to load/store Images from/to files.
 */
class ImageTools
{
public:
  /**
   * Loads an Image from a file.
   * @param path The path from which the Image should be loaded.
   * @return The loaded Image.
   */
  static Image loadImage(const std::string& path);
  /*+
   * Stores an Image to a file.
   * @param path The path where the Image should be stored.
   * @param image The Image to store.
   */
  static void storeImage(const std::string& path, const Image& image);
};
