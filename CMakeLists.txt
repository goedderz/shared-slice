cmake_minimum_required(VERSION 3.15)
project(shared_slice CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(velocypack)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_library(shared_slice
  src/SharedSlice.cpp src/SharedSlice.h)

target_link_libraries(shared_slice velocypack)

target_compile_options(shared_slice PRIVATE "-fsanitize=address")
target_compile_options(shared_slice PRIVATE "-fsanitize=undefined")
target_link_libraries(shared_slice ubsan)

