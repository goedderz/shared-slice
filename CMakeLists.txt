cmake_minimum_required(VERSION 3.15)
project(shared_slice CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(velocypack)
add_subdirectory(googletest)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include_directories(shared_slice src)
include_directories(tests tests)

add_library(shared_slice
  src/velocypack/SharedSlice.cpp src/velocypack/SharedSlice.h
  src/velocypack/SharedIterator.cpp src/velocypack/SharedIterator.h
  )

add_executable(tests
  tests/tests.cpp
  tests/cases/SharedSliceTest.cpp
  )

target_link_libraries(shared_slice velocypack)
target_link_libraries(tests gtest)
target_link_libraries(tests shared_slice)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  if (NOT MSVC)
    target_compile_options(shared_slice PRIVATE "-fsanitize=address")
    target_link_libraries(shared_slice asan)
    target_compile_options(shared_slice PRIVATE "-fsanitize=undefined")
    target_link_libraries(shared_slice ubsan)

    target_compile_options(tests PRIVATE "-fsanitize=address")
    target_link_libraries(tests asan)
    target_compile_options(tests PRIVATE "-fsanitize=undefined")
    target_link_libraries(tests ubsan)
  endif ()
endif ()
