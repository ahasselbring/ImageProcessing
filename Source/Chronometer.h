/**
 * @file Chronometer.h
 *
 * This file declares the Chronometer class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief This class is a utility for time measurements.
 */
class Chronometer final
{
public:
  /**
   * @brief Starts a time measurement (stopwatch).
   * @param str An identifier for the measurement.
   */
  explicit Chronometer(const std::string& str);
  /**
   * @brief Stops a time measurement and adds the duration to a list.
   */
  ~Chronometer();
  /**
   * @brief Prints the measured times.
   */
  static void printStats();
private:
  std::string str;                                                      ///< An identifier for the measurement.
  std::chrono::time_point<std::chrono::high_resolution_clock> start;    ///< The timestamp when the measurement has been started.
  static std::unordered_map<std::string, std::vector<double>> runtimes; ///< The global list of all runtimes.
};
