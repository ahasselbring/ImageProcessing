/**
 * @file IPSLBlackboard.h
 *
 * This file declares and implements the IPSLBlackboard class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <string>

#include "IPSLError.h"
#include "IPSLToken.h"

/**
 * @brief This class stores the data that are relevant to the IPSL execution process.
 */
class IPSLBlackboard final
{
public:
  /**
   * @brief Constructs a blackboard.
   * @param inputName The name of the input stream.
   * @param source The source code.
   */
  IPSLBlackboard(const std::string& inputName, const std::string& source) :
    inputName(inputName),
    source(source)
  {
  }
  std::string inputName;                     ///< The name of the input stream.
  std::string source;                        ///< The source code.
  IPSLTokenSequence tokenSequence;           ///< The token sequence (only valid after lexical analysis).
  IPSLErrorList errorList;                   ///< A list of errors.
};
