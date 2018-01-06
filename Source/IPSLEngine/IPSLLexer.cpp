/**
 * @file IPSLLexer.cpp
 *
 * This file implements the IPSLLexer class.
 *
 * @author Arne Hasselbring
 */

#include <cassert>
#include <cctype>
#include <string>

#include "IPSLLocation.h"
#include "IPSLToken.h"

#include "IPSLLexer.h"

void IPSLLexer::analyze(IPSLBlackboard& blackboard)
{
  std::string currentToken;
  unsigned int row = 1, column = 1, index = 0;
  State state = State::normal;
  const auto& source = blackboard.source;
  auto& tokenSequence = blackboard.tokenSequence;
  auto& errorList = blackboard.errorList;
  for(char c : source)
  {
    IPSLLocation location(row, column, index);
    switch(state)
    {
tryAgain:
      case State::normal:
        if(c == '-')
        {
          currentToken += c;
          state = State::expectNumberOrSymbol;
        }
        else if(std::isdigit(c))
        {
          currentToken += c;
          state = State::expectNumber;
        }
        else if(isSymbolCharacter(c))
        {
          currentToken += c;
          state = State::expectSymbol;
        }
        else if(c == '"')
          state = State::expectString;
        else if(c == ';')
          state = State::expectComment;
        else if(c == '(')
          tokenSequence.emplace_back(IPSLToken::Type::lParenthesis);
        else if(c == ')')
          tokenSequence.emplace_back(IPSLToken::Type::rParenthesis);
        else if(c == '{')
          tokenSequence.emplace_back(IPSLToken::Type::lBrace);
        else if(c == '}')
          tokenSequence.emplace_back(IPSLToken::Type::rBrace);
        else if(std::isspace(c))
          ;
        else
          errorList.emplace_back(location, "Unexpected symbol!");
        break;
      case State::expectNumberOrSymbol:
        assert(currentToken.length() == 1);
        if(!isSymbolCharacter(c))
        {
          if(currentToken[0] == '-')
          {
            tokenSequence.emplace_back(IPSLToken::Type::symbol);
            tokenSequence.back().value = currentToken;
          }
          else
          {
            assert(std::isdigit(currentToken[0]));
            tokenSequence.emplace_back(IPSLToken::Type::number);
            tokenSequence.back().value = currentToken;
          }
          currentToken.clear();
          state = State::normal;
          goto tryAgain;
        }
        currentToken += c;
        state = std::isdigit(c) ? State::expectNumber : State::expectSymbol;
        break;
      case State::expectNumber:
        if(!std::isdigit(c))
        {
          tokenSequence.emplace_back(IPSLToken::Type::number);
          tokenSequence.back().value = currentToken;
          currentToken.clear();
          state = State::normal;
          goto tryAgain;
        }
        currentToken += c;
        break;
      case State::expectSymbol:
        if(!isSymbolCharacter(c))
        {
          tokenSequence.emplace_back(IPSLToken::Type::symbol);
          tokenSequence.back().value = currentToken;
          currentToken.clear();
          state = State::normal;
          goto tryAgain;
        }
        currentToken += c;
        break;
      case State::expectString:
        if(c == '"')
        {
          tokenSequence.emplace_back(IPSLToken::Type::string);
          tokenSequence.back().value = currentToken;
          currentToken.clear();
          state = State::normal;
          break;
        }
        else if(c == '\\')
        {
          state = State::expectStringEscaped;
          break;
        }
        currentToken += c;
        break;
      case State::expectStringEscaped:
        if(c == '"' || c == '\\')
          currentToken += c;
        else if(c == 'n')
          currentToken += '\n';
        else if(c == 'r')
          currentToken += '\r';
        else if(c == 't')
          currentToken += '\t';
        else
          errorList.emplace_back(location, "Unknown escape sequence!");
        state = State::expectString;
        break;
      case State::expectComment:
        if(c == '\n')
        {
          tokenSequence.emplace_back(IPSLToken::Type::comment);
          tokenSequence.back().value = currentToken;
          currentToken.clear();
          state = State::normal;
          break;
        }
        currentToken += c;
        break;
      default:
        assert(false);
    }
    ++index;
    if(c == '\n')
    {
      ++row;
      column = 0;
    }
    ++column;
  }

  if(state != State::normal || !currentToken.empty())
    errorList.emplace_back(IPSLLocation(row, column, index), "Unexpected end of input!");
}

bool IPSLLexer::isSymbolCharacter(char c)
{
  return std::isalnum(c) || c == '_' || c == '+' || c  == '-' || c == '*' || c == '/'
    || c == '\\' || c == '=' || c == '<' || c == '>' || c == '!' || c == '&';
}
