/**
 * @file IPSLLexer.h
 *
 * This file declares the IPSLLexer class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include "IPSLBlackboard.h"

/**
 * @brief This class provides methods for lexical analysis of the source code.
 */
class IPSLLexer final
{
public:
  /**
   * @brief Converts a text string into a sequence of tokens.
   * @param blackboard The blackboard.
   */
  void analyze(IPSLBlackboard& blackboard);
private:
  /**
   * @brief This enum class enumerates the states of the DFA that parses the regular expressions.
   */
  enum class State
  {
    normal,               ///< The state when anything could be next.
    expectNumberOrSymbol, ///< The state when a sign has been read
    expectNumber,         ///< The state when a digit has been read.
    expectSymbol,         ///< The state when a symbol character has been read.
    expectString,         ///< The state when a quote has been read.
    expectStringEscaped,  ///< The state when a backslash has been read in a string.
    expectComment,        ///< The state when a semicolon has been read.
    numOfStates,          ///< The number of states.
  };
  /**
   * @brief Returns whether the character could belong to a symbol token.
   * @param c The character to test.
   * @return True iff the character could belong to a symbol token.
   */
  static bool isSymbolCharacter(char c);
};
