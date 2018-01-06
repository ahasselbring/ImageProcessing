/**
 * @file IPSLLocation.h
 *
 * This file declares and implements the IPSLLocation class.
 *
 * @author Arne Hasselbring
 */

#pragma once

/**
 * @brief This class specifies a location in a program.
 */
class IPSLLocation final
{
public:
  /**
   * @brief Constructs a location.
   * @param row The row (1-based).
   * @param column The column (1-based).
   * @param index The index in the source code string (0-based).
   */
  IPSLLocation(unsigned int row, unsigned int column, unsigned int index) :
    row(row),
    column(column),
    index(index)
  {
  }
  unsigned int row;    ///< The row (1-based).
  unsigned int column; ///< The column (1-based).
  unsigned int index;  ///< The index in the source code string (0-based).
};
