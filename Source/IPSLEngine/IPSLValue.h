/**
 * @file IPSLValue.h
 *
 * This file declares the IPSLValue class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <string>

#include "IPSLAbstractSyntaxTree.h"

class IPSLValue;

using IPSLValuePtr = std::unique_ptr<IPSLValue>;

class IPSLEnvironment;

using IPSLBuiltin = std::function<IPSLValuePtr(IPSLEnvironment&, IPSLValuePtr)>;

#include "IPSLEnvironment.h"

/**
 * @brief This class represents a runtime value in the IPSL interpreter.
 */
class IPSLValue
{
public:
  /**
   * @brief This enum class enumerates the types of values.
   */
  enum class Type
  {
    error,       ///< An error value.
    number,      ///< A number value.
    symbol,      ///< A symbol value.
    string,      ///< A string value.
    function,    ///< A function value.
    sExpression, ///< An S-Expression value.
    qExpression, ///< A Q-Expression value.
    numOfTypes   ///< The number of value types.
  };
  /**
   * @brief Constructs a value with a given type.
   * @param type The type of the new value.
   */
  explicit IPSLValue(Type type) :
    type(type)
  {
  }
  /**
   * @brief Copy constructor.
   * @param other The copied value.
   */
  IPSLValue(const IPSLValue& other);
  /**
   * @brief Appends a single value to the end of this value.
   * @param other The new added value.
   */
  void add(IPSLValuePtr other);
  /**
   * @brief Appends all sub-values of another value to the end of this value.
   * @param other The added collection of values.
   */
  void join(IPSLValuePtr other);
  /**
   * @brief Creates a copy of this value.
   * @return A pointer to the copy.
   */
  IPSLValuePtr copy() const;
  /**
   * @brief Prints this value.
   */
  void print() const;
  /**
   * @brief Prints this value with a line break in the end.
   */
  void println() const;
  /**
   * @brief Creates an error value.
   * @param errorValue The error.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newError(const std::string& errorValue);
  /**
   * @brief Creates a number value.
   * @param numberValue The number.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newNumber(std::uintptr_t numberValue);
  /**
   * @brief Creates a symbol value.
   * @param symbolValue The symbol.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newSymbol(const std::string& symbolValue);
  /**
   * @brief Creates a string value.
   * @param stringValue The string.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newString(const std::string& stringValue);
  /**
   * @brief Creates a function value for non-builtin functions.
   * @param builtinValue The builtin function.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newBuiltin(const IPSLBuiltin& builtinValue);
  /**
   * @brief Creates a function value for non-builtin functions.
   * @param formals The formals.
   * @param body The body.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newLambda(IPSLValuePtr formals, IPSLValuePtr body);
  /**
   * @brief Create a new (empty) S-Expression value.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newSExpression();
  /**
   * @brief Create a new (empty) Q-Expression value.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr newQExpression();
  /**
   * @brief Generates a value from an abstract syntax tree node.
   * @param astNode The node from which to generate the value.
   * @return A pointer to the new value.
   */
  static IPSLValuePtr fromAbstractSyntaxTree(const IPSLAbstractSyntaxTree::Node& astNode);
  /**
   * @brief Returns the name of a type as string.
   * @param type The type as enum class.
   * @return The type as string.
   */
  static std::string typeName(Type type);
  Type type;                                    ///< The type of this value.
  std::uintptr_t numberValue;                   ///< The numeric value of this value.
  std::string errorSymbolStringValue;           ///< The textual value of this value.
  IPSLBuiltin builtinValue;                     ///< The function handle for builtin functions.
  std::shared_ptr<IPSLEnvironment> environment; ///< The environment for non-builtin functions.
  IPSLValuePtr formals;                         ///< The description of the arguments for non-builtin functions.
  IPSLValuePtr body;                            ///< The function body for non-builtin functions.
  std::list<IPSLValuePtr> expressionValues;     ///< The sub-values of S- or Q-Expressions.
private:
  /**
   * @brief Prints an S- or Q-Expression.
   * @param open The opening parenthesis/brace.
   * @param close The closing parenthesis/brace.
   */
  void printExpr(char open, char close) const;
};
