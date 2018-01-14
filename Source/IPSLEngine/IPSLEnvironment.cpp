/**
 * @file IPSLEnvironment.cpp
 *
 * This file implements the IPSLEnvironment class.
 *
 * @author Arne Hasselbring
 */

#include "IPSLEnvironment.h"

IPSLEnvironment::IPSLEnvironment(const IPSLEnvironment& other) :
  std::enable_shared_from_this<IPSLEnvironment>(other),
  parent(other.parent)
{
  for(auto& pair : other.values)
    values[pair.first] = pair.second->copy();
}

std::shared_ptr<IPSLEnvironment> IPSLEnvironment::copy() const
{
  return std::make_shared<IPSLEnvironment>(*this);
}

void IPSLEnvironment::addBuiltin(const std::string& name, const IPSLBuiltin& builtin)
{
  put(*IPSLValue::newSymbol(name), IPSLValue::newBuiltin(builtin));
}

IPSLValuePtr IPSLEnvironment::get(const IPSLValue& key)
{
  auto it = values.find(key.errorSymbolStringValue);
  if(it != values.end())
    return it->second->copy();

  if(parent)
    return parent->get(key);
  else
    return IPSLValue::newError("Unbound symbol '" + key.errorSymbolStringValue + "'!");
}

void IPSLEnvironment::put(const IPSLValue& key, IPSLValuePtr value)
{
  values[key.errorSymbolStringValue] = std::move(value);
}

void IPSLEnvironment::def(const IPSLValue& key, IPSLValuePtr value)
{
  IPSLEnvironment* e = this;
  while (e->parent != nullptr)
    e = e->parent.get();
  put(key, std::move(value));
}

IPSLValuePtr IPSLEnvironment::eval(IPSLValuePtr value)
{
  if(value->type == IPSLValue::Type::symbol)
    return get(*value);
  else if(value->type == IPSLValue::Type::sExpression)
    return evalSExpression(std::move(value));
  return value;
}

IPSLValuePtr IPSLEnvironment::evalSExpression(IPSLValuePtr value)
{
  if(value->expressionValues.empty())
    return value;

  for(auto& ptr : value->expressionValues)
  {
    ptr = eval(std::move(ptr));
    if(ptr->type == IPSLValue::Type::error)
      return std::move(ptr);
  }

  if(value->expressionValues.size() == 1)
    return eval(std::move(value->expressionValues.front()));

  IPSLValuePtr f = std::move(value->expressionValues.front());
  value->expressionValues.pop_front();
  if(f->type != IPSLValue::Type::function)
    return IPSLValue::newError("S-Expression starts with incorrect type! Got "
      + IPSLValue::typeName(f->type) + ", expected "
      + IPSLValue::typeName(IPSLValue::Type::function) + '!');

  return call(std::move(f), std::move(value));
}

IPSLValuePtr IPSLEnvironment::call(IPSLValuePtr function, IPSLValuePtr arguments)
{
  if(function->builtinValue)
    return function->builtinValue(*this, std::move(arguments));

  return IPSLValue::newError("User-defined functions are not implemented yet!");
}
