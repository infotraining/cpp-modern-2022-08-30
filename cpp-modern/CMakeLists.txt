# Set VCPKG_ROOT to your vcpkg installation directory or add the following to your cmake call:
# -DCMAKE_TOOLCHAIN=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake

cmake_minimum_required(VERSION 3.16)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")  
endif()
if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

project(cpp-modern LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(structured-bindings)
add_subdirectory(ifs)
add_subdirectory(small-features)
add_subdirectory(type-deduction)
add_subdirectory(fold-expressions)
add_subdirectory(new-types-std-lib)
add_subdirectory(custom-literals)
add_subdirectory(traits-concepts)
add_subdirectory(constexpr)
add_subdirectory(ranges)
add_subdirectory(_exercise/ex_constexpr_if)