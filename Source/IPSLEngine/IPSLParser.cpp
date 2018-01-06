/**
 * @file IPSLParser.cpp
 *
 * This file implements the IPSLParser class.
 *
 * @author Arne Hasselbring
 */

#include <cassert>

#include "IPSLToken.h"

#include "IPSLParser.h"

void IPSLParser::parse(IPSLBlackboard& blackboard)
{
  tokenIterator = blackboard.tokenSequence.begin();
  tokenEnd = blackboard.tokenSequence.end();
  builder.init(blackboard.abstractSyntaxTree);
  atEndOfInput = false;

  try
  {
    nextToken();
    while(token.type != IPSLToken::Type::numOfTypes)
      parseExpression();
  }
  catch(const IPSLError& e)
  {
    blackboard.errorList.push_back(e);
  }
}

void IPSLParser::parseNumber()
{
  assert(token.type == IPSLToken::Type::number);
  builder.enter(IPSLAbstractSyntaxTree::Node::Type::number);
  builder.current().value = token.value;
  builder.leave();
  nextToken();
}

void IPSLParser::parseSymbol()
{
  assert(token.type == IPSLToken::Type::symbol);
  builder.enter(IPSLAbstractSyntaxTree::Node::Type::symbol);
  builder.current().value = token.value;
  builder.leave();
  nextToken();
}

void IPSLParser::parseString()
{
  assert(token.type == IPSLToken::Type::string);
  builder.enter(IPSLAbstractSyntaxTree::Node::Type::string);
  builder.current().value = token.value;
  builder.leave();
  nextToken();
}

void IPSLParser::parseComment()
{
  assert(token.type == IPSLToken::Type::comment);
  builder.enter(IPSLAbstractSyntaxTree::Node::Type::comment);
  builder.current().value = token.value;
  builder.leave();
  nextToken();
}

void IPSLParser::parseSExpression()
{
  assert(token.type == IPSLToken::Type::lParenthesis);
  nextToken();
  builder.enter(IPSLAbstractSyntaxTree::Node::Type::sExpression);
  while(token.type != IPSLToken::Type::rParenthesis)
    parseExpression();
  builder.leave();
  nextToken();
}

void IPSLParser::parseQExpression()
{
  assert(token.type == IPSLToken::Type::lBrace);
  nextToken();
  builder.enter(IPSLAbstractSyntaxTree::Node::Type::qExpression);
  while(token.type != IPSLToken::Type::rBrace)
    parseExpression();
  builder.leave();
  nextToken();
}

void IPSLParser::parseExpression()
{
  builder.enter(IPSLAbstractSyntaxTree::Node::Type::expression);
  switch(token.type)
  {
    case IPSLToken::Type::number:
      parseNumber();
      break;
    case IPSLToken::Type::symbol:
      parseSymbol();
      break;
    case IPSLToken::Type::string:
      parseString();
      break;
    case IPSLToken::Type::comment:
      parseComment();
      break;
    case IPSLToken::Type::lParenthesis:
      parseSExpression();
      break;
    case IPSLToken::Type::lBrace:
      parseQExpression();
      break;
    default:
      throw IPSLError(token.location, "Unexpected token!");
  }
  builder.leave();
}

void IPSLParser::nextToken()
{
  if(tokenIterator == tokenEnd)
  {
    if(!atEndOfInput)
      atEndOfInput = true;
    else
      assert(false);

    token = IPSLToken(IPSLToken::Type::numOfTypes, token.location);
    return;
  }

  token = *tokenIterator;
  tokenIterator++;
}
