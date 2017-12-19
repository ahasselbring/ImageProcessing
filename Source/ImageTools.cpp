/**
 * @file ImageTools.cpp
 *
 * This file implements the ImageTools class.
 *
 * @author Arne Hasselbring
 */

#include <vector>

#include <lodepng/lodepng.h>

#include "ImageTools.h"

Image ImageTools::loadImage(const std::string& path)
{
  std::vector<unsigned char> data;
  unsigned int width, height;
  lodepng::decode(data, width, height, path.c_str());
  Image result(width, height, (width % 32) == 0);
  for(unsigned int y = 0; y < height; y++)
    for(unsigned int x = 0; x < width; x++)
      result[y][x] = data[(y * width + x) * 4];
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
