/**
 * @file Operator.h
 *
 * This file declares the Operator class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <memory>

#include "Image.h"

/**
 * @brief This class provides an interface for image-transforming operators.
 */
class Operator
{
public:
  /**
   * @brief Virtual destructor for polymorphism.
   */
  virtual ~Operator() = default;
  /**
   * @brief Applies the operator to an image.
   * @param The operand.
   * @return The value.
   */
  virtual Image apply(const Image& image) = 0;
};

using OperatorPtr = std::unique_ptr<Operator>;
