/**
 * @file ImageTools.cpp
 *
 * This file implements the ImageTools class.
 *
 * @author Arne Hasselbring
 */

#include <stdexcept>
#include <vector>

#include <lodepng/lodepng.h>

#include "ImageTools.h"

Image ImageTools::loadImage(const std::string& path, ImageFormat format)
{
  std::vector<unsigned char> data;
  unsigned int width, height;
  lodepng::decode(data, width, height, path.c_str());
  Image result(width, height, (width % 32) == 0);
  switch(format)
  {
    case ImageFormat::PNG:
      for(unsigned int y = 0; y < height; y++)
        for(unsigned int x = 0; x < width; x++)
          result[y][x] = clamp(static_cast<int>(0.299 * data[(y * width + x) * 4] + 0.587 * data[(y * width + x) * 4 + 1] + 0.114 * data[(y * width + x) * 4 + 2] + 0.5));
      break;
    case ImageFormat::YCbCr:
      for(unsigned int y = 0; y < height; y++)
        for(unsigned int x = 0; x < width; x++)
          result[y][x] = data[(y * width + x) * 4];
      break;
    default:
      throw std::runtime_error("Unknown image format given!");
  }
  return result;
}

void ImageTools::storeImage(const std::string& path, const Image& image)
{
  std::vector<unsigned char> data(4 * image.width * image.height);
  for(unsigned int y = 0; y < image.height; y++)
    for(unsigned int x = 0; x < image.width; x++)
    {
      data[(y * image.width + x) * 4] = image[y][x];
      data[(y * image.width + x) * 4 + 1] = image[y][x];
      data[(y * image.width + x) * 4 + 2] = image[y][x];
      data[(y * image.width + x) * 4 + 3] = 255;
    }

  lodepng::encode(path.c_str(), data, image.width, image.height);
}

bool ImageTools::compare(const Image& image1, const Image& image2)
{
  if(image1.width != image2.width || image1.height != image2.height)
    return false;

  for(unsigned int y = 0; y < image1.height; y++)
    for(unsigned int x = 0; x < image1.width; x++)
      if(image1[y][x] != image2[y][x])
        return false;

  return true;
}

unsigned char ImageTools::clamp(int value)
{
  return (value > 255) ? 255 : ((value < 0) ? 0 : value);
}
