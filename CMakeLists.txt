cmake_minimum_required(VERSION 3.5)
project(ImageProcessing)

set(SOURCES
  3rdParty/lodepng/lodepng.cpp
  3rdParty/lodepng/lodepng.h
  Source/AlignedMemory.cpp
  Source/AlignedMemory.h
  Source/Avg5.cpp
  Source/Avg5.h
  Source/Chronometer.cpp
  Source/Chronometer.h
  Source/Image.h
  Source/ImageTools.cpp
  Source/ImageTools.h
  Source/IPSLEngine/IPSLAbstractSyntaxTree.h
  Source/IPSLEngine/IPSLBlackboard.h
  Source/IPSLEngine/IPSLError.cpp
  Source/IPSLEngine/IPSLError.h
  Source/IPSLEngine/IPSLLexer.cpp
  Source/IPSLEngine/IPSLLexer.h
  Source/IPSLEngine/IPSLLocation.h
  Source/IPSLEngine/IPSLToken.h
  Source/Main.cpp
  Source/OptimizationLevel.h
  Source/PeronaMalik.cpp
  Source/PeronaMalik.h
  Source/SIMD.h
)

add_executable(filters ${SOURCES})
target_include_directories(filters SYSTEM PRIVATE 3rdParty)
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  target_compile_options(filters PRIVATE -march=native -Wall -Wextra -pedantic)
endif()
set_target_properties(filters
  PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
