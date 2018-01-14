/**
 * @file IPSLValue.cpp
 *
 * This file implements the IPSLValue class.
 *
 * @author Arne Hasselbring
 */

#include <cassert>
#include <iostream>

#include "IPSLValue.h"

IPSLValue::IPSLValue(const IPSLValue& other) :
  type(other.type)
{
  if(type == Type::error || type == Type::symbol || type == Type::string)
    errorSymbolStringValue = other.errorSymbolStringValue;
  else if(type == Type::number)
    numberValue = other.numberValue;
  else if(type == Type::function)
  {
    builtinValue = other.builtinValue;
    if(!builtinValue)
    {
      environment = other.environment->copy();
      formals = other.formals->copy();
      body = other.body->copy();
    }
  }
  else if(type == Type::sExpression || type == Type::qExpression)
  {
    for(auto& ptr : other.expressionValues)
      expressionValues.push_back(ptr->copy());
  }
  else
    assert(false);
}

void IPSLValue::add(IPSLValuePtr other)
{
  expressionValues.push_back(std::move(other));
}

void IPSLValue::join(IPSLValuePtr other)
{
  expressionValues.splice(expressionValues.end(), other->expressionValues);
}

IPSLValuePtr IPSLValue::copy() const
{
  return std::make_unique<IPSLValue>(*this);
}

void IPSLValue::print() const
{
  switch(type)
  {
    case Type::error:
      std::cout << "Error: " << errorSymbolStringValue;
      break;
    case Type::number:
      std::cout << numberValue;
      break;
    case Type::symbol:
      std::cout << errorSymbolStringValue;
      break;
    case Type::string:
      std::cout << '"' << errorSymbolStringValue << '"';
      break;
    case Type::function:
      if(builtinValue)
        std::cout << "<builtin>";
      else
      {
        std::cout << "\\ ";
        formals->print();
        std::cout << ' ';
        body->print();
      }
      break;
    case Type::sExpression:
      printExpr('(', ')');
      break;
    case Type::qExpression:
      printExpr('{', '}');
      break;
    default:
      assert(false);
  }
}

void IPSLValue::println() const
{
  print();
  std::cout << '\n';
}

void IPSLValue::printExpr(char open, char close) const
{
  std::cout << open;
  auto notLast = expressionValues.size();
  for(auto& ptr : expressionValues)
  {
    ptr->print();
    if(--notLast)
      std::cout << ' ';
  }
  std::cout << close;
}

IPSLValuePtr IPSLValue::newError(const std::string& errorValue)
{
  IPSLValuePtr result = std::make_unique<IPSLValue>(Type::error);
  result->errorSymbolStringValue = errorValue;
  return result;
}

IPSLValuePtr IPSLValue::newNumber(std::uintptr_t numberValue)
{
  IPSLValuePtr result = std::make_unique<IPSLValue>(Type::number);
  result->numberValue = numberValue;
  return result;
}

IPSLValuePtr IPSLValue::newSymbol(const std::string& symbolValue)
{
  IPSLValuePtr result = std::make_unique<IPSLValue>(Type::symbol);
  result->errorSymbolStringValue = symbolValue;
  return result;
}

IPSLValuePtr IPSLValue::newString(const std::string& stringValue)
{
  IPSLValuePtr result = std::make_unique<IPSLValue>(Type::string);
  result->errorSymbolStringValue = stringValue;
  return result;
}

IPSLValuePtr IPSLValue::newBuiltin(const IPSLBuiltin& builtinValue)
{
  IPSLValuePtr result = std::make_unique<IPSLValue>(Type::function);
  result->builtinValue = builtinValue;
  return result;
}

IPSLValuePtr IPSLValue::newLambda(IPSLValuePtr formals, IPSLValuePtr body)
{
  IPSLValuePtr result = std::make_unique<IPSLValue>(Type::function);
  result->environment = std::make_shared<IPSLEnvironment>();
  result->formals = std::move(formals);
  result->body = std::move(body);
  return result;
}

IPSLValuePtr IPSLValue::newSExpression()
{
  return std::make_unique<IPSLValue>(Type::sExpression);
}

IPSLValuePtr IPSLValue::newQExpression()
{
  return std::make_unique<IPSLValue>(Type::qExpression);
}

IPSLValuePtr IPSLValue::fromAbstractSyntaxTree(const IPSLAbstractSyntaxTree::Node& astNode)
{
  if(astNode.type == IPSLAbstractSyntaxTree::Node::Type::number)
    return newNumber(std::stoul(astNode.value));
  else if(astNode.type == IPSLAbstractSyntaxTree::Node::Type::symbol)
    return newSymbol(astNode.value);
  else if(astNode.type == IPSLAbstractSyntaxTree::Node::Type::string)
    return newString(astNode.value);
  else if(astNode.type == IPSLAbstractSyntaxTree::Node::Type::comment)
    return newString(astNode.value);
  else if(astNode.type == IPSLAbstractSyntaxTree::Node::Type::expression)
  {
    assert(astNode.children.size() == 1);
    return fromAbstractSyntaxTree(astNode.children[0]);
  }
  IPSLValuePtr expression = astNode.type == IPSLAbstractSyntaxTree::Node::Type::qExpression
    ? newQExpression() : newSExpression();
  for(auto& child : astNode.children)
    expression->expressionValues.push_back(fromAbstractSyntaxTree(child));
  return expression;
}

std::string IPSLValue::typeName(Type type)
{
  switch(type)
  {
    case Type::error:
      return "Error";
    case Type::number:
      return "Number";
    case Type::symbol:
      return "Symbol";
    case Type::string:
      return "String";
    case Type::function:
      return "Function";
    case Type::sExpression:
      return "S-Expression";
    case Type::qExpression:
      return "Q-Expression";
    default:
      assert(false);
      return "Unknown";
  }
}
