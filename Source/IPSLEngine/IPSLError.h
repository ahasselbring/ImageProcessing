/**
 * @file IPSLError.h
 *
 * This file declares the IPSLError class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <string>
#include <vector>

#include "IPSLLocation.h"

class IPSLBlackboard;

/**
 * @brief This class represents an error during the execution process.
 */
class IPSLError final
{
public:
  /**
   * @brief Constructs an error.
   * @param location The location in the source code at which the error occurred.
   * @param message An error-specific message with details about the error.
   */
  IPSLError(const IPSLLocation& location, const std::string& message);
  /**
   * @brief Returns a text string describing the error.
   * @return A text string describing the error.
   */
  std::string str(const IPSLBlackboard& blackboard) const;
private:
  IPSLLocation location; ///< The location in the source code at which the error occurred.
  std::string message;   ///< An error-specific message with details about the error.
};

using IPSLErrorList = std::vector<IPSLError>;
