/**
* @file AlignedMemory.cpp
*
* This file implements the AlignedMemory class.
*
* @author Arne Hasselbring
*/

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <cstdlib>
#endif

#include "AlignedMemory.h"

void* AlignedMemory::alloc(std::size_t size, std::size_t alignment)
{
#ifdef _WIN32
  return _aligned_malloc(size, alignment);
#else
  void* ptr;
  if(posix_memalign(&ptr, alignment, size) != 0)
    return nullptr;
  return ptr;
#endif
}

void AlignedMemory::free(void* ptr)
{
#ifdef _WIN32
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}