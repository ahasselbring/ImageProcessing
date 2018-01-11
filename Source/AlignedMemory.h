/**
 * @file AlignedMemory.h
 *
 * This file declares the AlignedMemory class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <cstdlib>

/**
 * @brief This class provides function to allocate and free aligned memory.
 */
class AlignedMemory
{
public:
  /**
   * @brief Allocates memory that has a specific alignment.
   * @param size The size of the requested memory.
   * @param alignment The resulting address must be a multiple of this.
   * @return A pointer to the allocated memory.
   */
  static void* alloc(std::size_t size, std::size_t alignment);
  /**
   * @brief Frees memory that has been allocated with alloc.
   * @param A pointer to the memory that shall be freed.
   */
  static void free(void* ptr);
};
