/**
 * @file IPSLParser.h
 *
 * This file declares the IPSLParser class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include "IPSLAbstractSyntaxTreeBuilder.h"
#include "IPSLBlackboard.h"
#include "IPSLToken.h"

/**
 * @brief This class implements a recursive descent parser for the IPS language.
 */
class IPSLParser final
{
public:
  /**
   * @brief Performs semantic analysis and converts a token sequence into an abstract syntax tree.
   * @param blackboard The blackboard.
   */
  void parse(IPSLBlackboard& blackboard);
private:
  /**
   * @brief Parses a number.
   */
  void parseNumber();
  /**
   * @brief Parses a symbol.
   */
  void parseSymbol();
  /**
   * @brief Parses a string.
   */
  void parseString();
  /**
   * @brief Parses a comment.
   */
  void parseComment();
  /**
   * @brief Parses an S-Expression.
   */
  void parseSExpression();
  /**
   * @brief Parses a Q-Expression.
   */
  void parseQExpression();
  /**
   * @brief Parses an expression.
   */
  void parseExpression();
  /**
   * @brief Advances to the next token.
   */
  void nextToken();
  IPSLTokenSequence::const_iterator tokenIterator; ///< An iterator pointing to the next token.
  IPSLTokenSequence::const_iterator tokenEnd;      ///< An iterator pointing to the end of the token sequence.
  IPSLAbstractSyntaxTreeBuilder builder;           ///< The builder that builds the abstract syntax tree.
  IPSLToken token = IPSLToken::invalid();          ///< The current token.
  bool atEndOfInput = false;                       ///< Whether the last token of the sequence has been read.
};
