/**
 * @file IPSLEnvironment.h
 *
 * This file declares the IPSLEnvironment class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "IPSLValue.h"

/**
 * @brief This class represents the environment in which expressions are evaluated.
 */
class IPSLEnvironment : public std::enable_shared_from_this<IPSLEnvironment>
{
public:
  /**
   * @brief Creates an empty environment.
   */
  IPSLEnvironment() = default;
  /**
   * @brief Copy constructor.
   * @param other The copied environment.
   */
  IPSLEnvironment(const IPSLEnvironment& other);
  /**
   * @brief Creates a copy of this environment.
   * @return A pointer to the copy.
   */
  std::shared_ptr<IPSLEnvironment> copy() const;
  /**
   * @brief Adds a builtin function to the environment.
   * @param name The name of the function.
   * @param builtin A handle to the function.
   */
  void addBuiltin(const std::string& name, const IPSLBuiltin& builtin);
  /**
   * @brief Returns a copy of the value with a given name.
   * @param key A symbol that contains the name.
   * @return A copy of the value with that name.
   */
  IPSLValuePtr get(const IPSLValue& key);
  /**
   * @brief Binds a value to a name in the current environment.
   * @param key A symbol that contains the name.
   * @param value The value that is bound to that name.
   */
  void put(const IPSLValue& key, IPSLValuePtr value);
  /**
   * @brief Binds a value to a name in the root environment.
   * @param key A symbol that contains the name.
   * @param value The value that is bound to that name.
   */
  void def(const IPSLValue& key, IPSLValuePtr value);
  /**
   * @brief Evaluates a value in this environment.
   * @param value The value that is evaluated.
   * @return The return value of the evaluation.
   */
  IPSLValuePtr eval(IPSLValuePtr value);
private:
  /**
   * @brief Evaluates an S-Expression.
   * @param value The value that is evaluated (must be an S-Expression).
   * @return The return value of the evaluation.
   */
  IPSLValuePtr evalSExpression(IPSLValuePtr value);
  /**
   * @brief Calls a function.
   * @param function The function to be called.
   * @param arguments The arguments that are passed to the function.
   * @return The return value of the function.
   */
  IPSLValuePtr call(IPSLValuePtr function, IPSLValuePtr arguments);
  std::shared_ptr<IPSLEnvironment> parent;              ///< The parent environment, if any.
  std::unordered_map<std::string, IPSLValuePtr> values; ///< The values defined in this environment.
};
