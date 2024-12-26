cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(ccmath-test)

if (NOT TARGET ccmath)
  find_package(ccmath CONFIG REQUIRED)
endif ()

# Include Abseil
find_package(Abseil REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(ccmath::test ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC
        ccmath::ccmath
        gtest::gtest
        ${Abseil_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC ${Abseil_INCLUDE_DIRS})
target_sources(${PROJECT_NAME} PRIVATE
        ccmath_test_main.cpp
)


add_executable(${PROJECT_NAME}-power)
target_sources(${PROJECT_NAME}-power PRIVATE
        power/sqrt_test.cpp
)
target_link_libraries(${PROJECT_NAME}-power PRIVATE
        ccmath::test
        gtest::gtest
        ${Abseil_LIBRARIES}
)


if (CCMATH_OS_WINDOWS)
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()

# Only supported compilers currently are MSVC, GNU and Clang
if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC
          /W4 /WX
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  target_compile_options(${PROJECT_NAME} PUBLIC
          -Wall -Wextra -Wno-pedantic -Wno-unused-function
  )
endif ()



add_test(NAME ${PROJECT_NAME}-power COMMAND ${PROJECT_NAME}-power)
