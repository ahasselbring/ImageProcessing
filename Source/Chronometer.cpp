/**
 * @file Chronometer.cpp
 *
 * This file implements the Chronometer class.
 *
 * @author Arne Hasselbring
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include "Chronometer.h"

std::unordered_map<std::string, std::vector<double>> Chronometer::runtimes;

Chronometer::Chronometer(const std::string& str) :
  str(str),
  start(std::chrono::high_resolution_clock::now())
{
}

Chronometer::~Chronometer()
{
  auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
  runtimes[str].push_back(static_cast<double>(microseconds.count()));
}

void Chronometer::printStats()
{
  std::cout << "Runtimes:\n";
  for(auto& runtime : runtimes)
  {
    using SizeType = std::vector<double>::size_type;
    auto& vec = runtime.second;
    SizeType startIndex = 0, endIndex = vec.size();
    if(vec.size() > 3)
    {
      const SizeType throwAwayMeasurements = std::max(static_cast<std::vector<double>::size_type>(1), vec.size() / 10);
      std::sort(vec.begin(), vec.end());
      startIndex = throwAwayMeasurements;
      endIndex -= throwAwayMeasurements;
    }
    double mean = 0;
    for(SizeType i = startIndex; i < endIndex; i++)
      mean += vec[i];
    mean /= (endIndex - startIndex);
    double stddev = 0;
    for(SizeType i = startIndex; i < endIndex; i++)
      stddev += (vec[i] - mean) * (vec[i] - mean);
    stddev /= (endIndex - startIndex);
    assert(stddev >= 0);
    stddev = std::sqrt(stddev);
    assert(std::isfinite(stddev));
    std::cout << "  " << runtime.first << ": Mean is " << mean << "us (Standard Deviation is " << stddev << "us).\n";
  }
}
