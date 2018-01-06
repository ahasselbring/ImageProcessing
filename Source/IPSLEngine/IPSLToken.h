/**
 * @file IPSLToken.h
 *
 * This file declares and implements the IPSLToken class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <string>
#include <vector>

/**
 * @brief This class represents a token in the IPS language.
 */
class IPSLToken final
{
public:
  /**
   * @brief This enum class enumerates the types of tokens.
   */
  enum class Type
  {
    number,       ///< A number token.
    symbol,       ///< A symbol token.
    string,       ///< A string token.
    comment,      ///< A comment token.
    lParenthesis, ///< A left parenthesis token.
    rParenthesis, ///< A right parenthesis token.
    lBrace,       ///< A left brace token.
    rBrace,       ///< A right brace token.
    numOfTypes    ///< The number of token types.
  };
  Type type;         ///< The type of the token.
  std::string value; ///< The value of the token (only valid for numbers, symbols, strings and comments).
  /**
   * @brief Constructs an IPSLToken.
   * @param type The type of the token.
   */
  explicit IPSLToken(Type type) :
    type(type)
  {
  }
};

using IPSLTokenSequence = std::vector<IPSLToken>;
