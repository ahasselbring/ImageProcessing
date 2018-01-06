/**
 * @file IPSLError.cpp
 *
 * This file implements the IPSLError class.
 *
 * @author Arne Hasselbring
 */

#include <sstream>

#include "IPSLBlackboard.h"

#include "IPSLError.h"

IPSLError::IPSLError(const IPSLLocation& location, const std::string& message) :
  location(location),
  message(message)
{
}

std::string IPSLError::str(const IPSLBlackboard& blackboard) const
{
  std::stringstream ss;
  ss << "In file " << blackboard.inputName
     << ", row " << location.row
     << ", column " << location.column
     << ":\n"
     << message
     << '\n';

  return ss.str();
}
