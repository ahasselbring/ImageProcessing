/**
 * @file Main.cpp
 *
 * This file declares the main procedure of the program.
 *
 * @author Arne Hasselbring
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Chronometer.h"
#include "PeronaMalik.h"
#include "Image.h"
#include "ImageTools.h"

int main(int argc, char* argv[])
{
  float kappa = 1.f, dt = 1.f;
  unsigned int times = 300;
  bool isotropic = false;

  if(argc < 2)
    return EXIT_FAILURE;

  for(int i = 2; i < argc; i++)
  {
    if(!strcmp(argv[i], "-kappa"))
    {
      if(++i == argc)
        return EXIT_FAILURE;
      kappa = static_cast<float>(atof(argv[i]));
    }
    else if(!strcmp(argv[i], "-dt"))
    {
      if(++i == argc)
        return EXIT_FAILURE;
      dt = static_cast<float>(atof(argv[i]));
    }
    else if(!strcmp(argv[i], "-times"))
    {
      if(++i == argc)
        return EXIT_FAILURE;
      times = atoi(argv[i]);
    }
    else if(!strcmp(argv[i], "-isotropic"))
      isotropic = !isotropic;
    else
      return EXIT_FAILURE;
  }

  Image image = ImageTools::loadImage(argv[1], ImageFormat::YCbCr);

  PeronaMalik pmAVX(kappa, dt, times, isotropic, OptimizationLevel::avx2);
  PeronaMalik pmSSE(kappa, dt, times, isotropic, OptimizationLevel::sse4);

  for(unsigned int i = 0; i < 100; i++)
  {
    Image result = pmAVX.apply(image);

    if(i)
      continue;

    Image result2 = pmSSE.apply(image);
    if(!ImageTools::compare(result, result2))
      std::cout << "Images are different!\n";

    ImageTools::storeImage("input.png", image);
    ImageTools::storeImage("output.png", result);
  }

  Chronometer::printStats();

  return EXIT_SUCCESS;
}
