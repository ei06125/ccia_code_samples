# #############################################################################
# Copyright (C) 2023 Pedro Oliveira. All Rights Reserved.
# #############################################################################

# #############################################################################
# Configuring
# #############################################################################

# =============================================================================
# CMake General Settings
# =============================================================================

cmake_minimum_required(VERSION 3.16)

set(COMPANY_NAME "ei06125")
project(
  ccia # C++ Concurrency In Action
  VERSION 1.0.0.0
  LANGUAGES CXX
)

# -----------------------------------------------------------------------------
# CMake Include Modules
# -----------------------------------------------------------------------------

include(FetchContent)
include(tools/cmake/Modules/BuildToolsMain.cmake)

# -----------------------------------------------------------------------------
# CMake Cache Variables & Options
# -----------------------------------------------------------------------------

cmake_host_system_information(RESULT OS_NAME_RESULT QUERY OS_NAME)

# short alias for top-level root folder
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# VRVIU is not using the latest standard
set(CMAKE_CXX_STANDARD 20)

# prevent std=gnu++20 or similar
set(CMAKE_CXX_EXTENSIONS OFF)

# Surpress CTest Targets (Continuous, Experimental, Nightly)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

# Surpresses MSVC Target ZERO_CHECK
set(CMAKE_SUPPRESS_REGENERATION ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_DEBUG_POSTFIX d)

# Check to see if this project is the root/top-level project
if(CMAKE_VERSION VERSION_LESS 3.21)
  # This variable is set by project() in CMake 3.21+
  string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}" PROJECT_IS_TOP_LEVEL)
endif()

# #############################################################################
# Building
# #############################################################################

# =============================================================================
# CMake Configured Headers
# =============================================================================

# configure project metadata header file based on CMake configuration options
include(tools/cmake/Templates/IncludeMeta.cmake)
configure_meta_header_file()

# =============================================================================
# Dependencies
# =============================================================================

# =============================================================================
# Pre-Compiled Headers
# =============================================================================

# C Standard Library pre-compiled headers
set(c_standard_library_pch # <cassert>
    <cmath>
    # <climits> <cstdint>
    <cstdio> <cstdlib> <cstring>
)

# C++ Standard Library pre-compiled headers
set(cpp_standard_library_pch
    <atomic>
    <algorithm>
    <condition_variable>
    <iostream>
    <execution>
    <functional>
    <future>
    # <filesystem>
    <memory>
    <mutex>
    <list>
    <queue>
    <random>
    <span>
    <string_view>
    <string>
    <sstream>
    <thread>
    <unordered_map>
    <utility>
    <vector>
)
# Win32 API pre-compiled headers
set(windows_api_library_pch #
    # <Windows.h> <WinSock2.h> <Ws2tcpip.h> <iphlpapi.h>
)

# POSIX API pre-compiled headers
set(linux_api_library_pch
    # <arpa/inet.h>
    # <errno.h>
    # <fcntl.h>
    # <ifaddrs.h>
    # <netdb.h>
    # <netinet/in.h>
    # <sys/socket.h>
    # <sys/types.h>
    # <unistd.h>
)

# =============================================================================
# global settings that propagate options/definitions/features to linking targets
# =============================================================================

add_library(ccia_global_settings INTERFACE)
set_project_warnings(ccia_global_settings)

# Include What You Use (from Windows)
if(OS_NAME_RESULT STREQUAL "Windows")

  target_compile_definitions(
    ccia_global_settings #
    INTERFACE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0601 NOMINMAX
  )

  # target_link_libraries(ccia_global_settings INTERFACE ws2_32)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ENABLE_SANITIZER_ADDRESS TRUE)
    enable_sanitizers(ccia_global_settings)
  endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(ccia_global_settings INTERFACE WOVEN_LOGGING_ENABLED)
endif()

# =============================================================================
# ccia: My Project main library
# =============================================================================

add_library(ccialib)

file(GLOB_RECURSE ccialib_SOURCE_FILES "lib/*.cpp")
file(GLOB_RECURSE ccialib_HEADER_FILES "lib/*.h")
file(GLOB_RECURSE ccialib_PUBLIC_HEADER_FILES "include/*.h")

target_sources(ccialib PRIVATE ${ccialib_SOURCE_FILES} ${ccialib_HEADER_FILES} ${ccialib_PUBLIC_HEADER_FILES})

target_include_directories(
  ccialib
  PRIVATE ${ROOT_DIR}/lib
  PUBLIC ${ROOT_DIR}/include
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(ccialib PUBLIC ccia_global_settings fmt::fmt spdlog::spdlog)

if(OS_NAME_RESULT STREQUAL "Windows")
  target_precompile_headers(
    ccialib
    PRIVATE
    # C Standard Library pre-compiled headers
    ${c_standard_library_pch}
    # C++ Standard Library pre-compiled headers
    ${cpp_standard_library_pch}
    # Win32 API pre-compiled headers
    ${windows_api_library_pch}
  )
elseif(OS_NAME_RESULT STREQUAL "Linux")
  target_precompile_headers(
    ccialib
    PRIVATE
    # C Standard Library pre-compiled headers
    ${c_standard_library_pch}
    # C++ Standard Library pre-compiled headers
    ${cpp_standard_library_pch}
    # POSIX API pre-compiled headers
    ${linux_api_library_pch}
  )
else()
  log_fatal("[BUG] OS_NAME_RESULT: ${OS_NAME_RESULT}")
endif()

# =============================================================================
# cciaapp: My Project main application
# =============================================================================

add_executable(cciaapp)

file(GLOB_RECURSE cciaapp_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE cciaapp_HEADER_FILES "src/*.h")

target_sources(cciaapp PRIVATE ${cciaapp_SOURCE_FILES} ${cciaapp_HEADER_FILES})

target_link_libraries(cciaapp PRIVATE ccialib)

if(OS_NAME_RESULT STREQUAL "Windows")
  target_precompile_headers(
    cciaapp
    PRIVATE
    # C Standard Library pre-compiled headers
    ${c_standard_library_pch}
    # C++ Standard Library pre-compiled headers
    ${cpp_standard_library_pch}
    # Win32 API pre-compiled headers
    ${windows_api_library_pch}
  )
elseif(OS_NAME_RESULT STREQUAL "Linux")
  target_precompile_headers(
    cciaapp
    PRIVATE
    # C Standard Library pre-compiled headers
    ${c_standard_library_pch}
    # C++ Standard Library pre-compiled headers
    ${cpp_standard_library_pch}
    # POSIX API pre-compiled headers
    ${linux_api_library_pch}
  )
else()
  log_fatal("[BUG] OS_NAME_RESULT: ${OS_NAME_RESULT}")
endif()

# #############################################################################
# Testing
# #############################################################################

enable_testing()
include(CTest)

# =============================================================================
# Main Tests
# =============================================================================

option(ccia_BUILD_MAIN_TESTS "Build main test targets" ${PROJECT_IS_TOP_LEVEL})
if(ccia_BUILD_MAIN_TESTS)
  # ---------------------------------------------------------------------------
  # Dependencies
  # ---------------------------------------------------------------------------
  find_package(Catch2 CONFIG REQUIRED)
  include(Catch)
  # ---------------------------------------------------------------------------
  # TARGET: ccia_main_tests
  # ---------------------------------------------------------------------------
  add_executable(ccia_main_tests)

  file(GLOB_RECURSE ccia_main_tests_SOURCE_FILES "tests/MainTests/*.cpp")

  target_sources(ccia_main_tests PRIVATE ${ccia_main_tests_SOURCE_FILES})

  target_include_directories(ccia_main_tests PRIVATE ${ROOT_DIR}/lib)

  target_link_libraries(ccia_main_tests PRIVATE ccialib Catch2::Catch2WithMain)

  if(OS_NAME_RESULT STREQUAL "Windows")
    target_precompile_headers(
      ccia_main_tests
      PRIVATE
      # Windows API
      ${windows_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  elseif(OS_NAME_RESULT STREQUAL "Linux")
    target_precompile_headers(
      ccia_main_tests
      PRIVATE
      # Linux API
      ${linux_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  else()
    log_fatal("[BUG] OS_NAME_RESULT: ${OS_NAME_RESULT}")
  endif()

  # ---------------------------------------------------------------------------
  # Utilities
  # ---------------------------------------------------------------------------

  catch_discover_tests(ccia_main_tests TEST_PREFIX "ccia_main_test_")

endif(ccia_BUILD_MAIN_TESTS)

# =============================================================================
# Unit Tests
# =============================================================================

option(ccia_BUILD_UNIT_TESTS "Build unit test targets" ${PROJECT_IS_TOP_LEVEL})
if(ccia_BUILD_UNIT_TESTS)
  # ---------------------------------------------------------------------------
  # Dependencies
  # ---------------------------------------------------------------------------
  set(gtest_EXTERNAL_DIR ${CMAKE_CURRENT_LIST_DIR}/external/googletest)
  find_package(GTest CONFIG REQUIRED)
  include(GoogleTest)
  # ---------------------------------------------------------------------------
  # TARGET: ccia_unit_tests
  # ---------------------------------------------------------------------------
  add_executable(ccia_unit_tests)

  file(GLOB_RECURSE ccia_unit_tests_SOURCE_FILES "tests/UnitTests/*.cpp")

  target_sources(ccia_unit_tests PRIVATE ${ccia_unit_tests_SOURCE_FILES})

  target_include_directories(ccia_unit_tests PRIVATE ${ROOT_DIR}/lib)

  target_link_libraries(ccia_unit_tests PRIVATE ccialib GTest::gtest GTest::gmock)

  if(OS_NAME_RESULT STREQUAL "Windows")
    target_precompile_headers(
      ccia_unit_tests
      PRIVATE
      # Windows API
      ${windows_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  elseif(OS_NAME_RESULT STREQUAL "Linux")
    target_precompile_headers(
      ccia_unit_tests
      PRIVATE
      # Linux API
      ${linux_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  else()
    log_fatal("[BUG] OS_NAME_RESULT: ${OS_NAME_RESULT}")
  endif()
  # ---------------------------------------------------------------------------
  # Utilities
  # ---------------------------------------------------------------------------
  gtest_discover_tests(ccia_unit_tests TEST_PREFIX "ccia_unit_test_")

endif(ccia_BUILD_UNIT_TESTS)

# =============================================================================
# Performance Tests
# =============================================================================

option(ccia_BUILD_PERF_TESTS "Build performance test target" ${PROJECT_IS_TOP_LEVEL})
if(ccia_BUILD_PERF_TESTS AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
  # ---------------------------------------------------------------------------
  # Dependencies
  # ---------------------------------------------------------------------------
  find_package(benchmark CONFIG REQUIRED)
  # ---------------------------------------------------------------------------
  # TARGET: ccia_perf_tests
  # ---------------------------------------------------------------------------
  add_executable(ccia_perf_tests)
  file(GLOB_RECURSE ccia_perf_tests_SOURCE_FILES "tests/PerfTests/*.cpp")

  target_sources(ccia_perf_tests PRIVATE ${ccia_perf_tests_SOURCE_FILES})

  target_link_libraries(ccia_perf_tests PRIVATE benchmark::benchmark)

  target_include_directories(ccia_perf_tests PRIVATE ${ROOT_DIR}/lib)

  if(OS_NAME_RESULT STREQUAL "Windows")
    target_precompile_headers(
      ccia_perf_tests
      PRIVATE
      # Windows API
      ${windows_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  elseif(OS_NAME_RESULT STREQUAL "Linux")
    target_precompile_headers(
      ccia_perf_tests
      PRIVATE
      # Linux API
      ${linux_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  else()
    log_fatal("[BUG] OS_NAME_RESULT: ${OS_NAME_RESULT}")
  endif()

  # ---------------------------------------------------------------------------
  # Utilities
  # ---------------------------------------------------------------------------
  # # Allow short runs during automated testing to see if something new breaks

  # set(PERF_RUNTIME 2 CACHE STRING "Number of seconds to run fuzz tests during ctest run" ) # Default of 2 seconds

  # add_test(NAME perf_tester_run COMMAND ccia_perf_tests -max_total_time=${PERF_RUNTIME} )
endif()

# =============================================================================
# Fuzzing Tests
# =============================================================================

option(ccia_BUILD_FUZZ_TESTS "Build fuzz test target" OFF) # ${PROJECT_IS_TOP_LEVEL})
if(ccia_BUILD_FUZZ_TESTS)
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR MSVC)
    log_warn("Fuzz tests require Clang")
    add_custom_target(ccia_fuzz_tests COMMAND ${CMAKE_COMMAND} -E echo "Fuzz tests require Clang")
  else()
    # -------------------------------------------------------------------------
    # Dependencies
    # -------------------------------------------------------------------------    
    find_package(fmt CONFIG REQUIRED)
    
    # -------------------------------------------------------------------------
    # TARGET: ccia_fuzz_tests
    # -------------------------------------------------------------------------
    add_executable(ccia_fuzz_tests)
    file(GLOB_RECURSE ccia_fuzz_tests_SOURCE_FILES "tests/FuzzTests/*.cpp")
    target_sources(ccia_fuzz_tests PRIVATE ${ccia_fuzz_tests_SOURCE_FILES})
    target_link_libraries(ccia_fuzz_tests PRIVATE ccia_global_settings fmt::fmt -coverage -fsanitize=fuzzer,undefined,address)
    target_compile_options(ccia_fuzz_tests PRIVATE -fsanitize=fuzzer,undefined,address)
    target_compile_features(ccia_fuzz_tests PRIVATE cxx_std_20)

    # add pre-compiled-headers
    if(OS_NAME_RESULT STREQUAL "Windows")
      target_precompile_headers(
        ccia_fuzz_tests
        PRIVATE
        # Windows API
        ${windows_api_library_pch}
        # C++ Standard Library
        ${cpp_standard_library_pch}
        # C Standard Library
        ${c_standard_library_pch}
      )
    elseif(OS_NAME_RESULT STREQUAL "Linux")
      target_precompile_headers(
        ccia_fuzz_tests
        PRIVATE
        # Linux API
        ${linux_api_library_pch}
        # C++ Standard Library
        ${cpp_standard_library_pch}
        # C Standard Library
        ${c_standard_library_pch}
      )
    else()
      log_fatal("[BUG] OS_NAME_RESULT: ${OS_NAME_RESULT}")
    endif()

    # -------------------------------------------------------------------------
    # Utilities
    # -------------------------------------------------------------------------
    # # Allow short runs during automated testing to see if something new breaks

    # set(FUZZ_RUNTIME 2 CACHE STRING "Number of seconds to run fuzz tests during ctest run" ) # Default of 2 seconds

    # add_test(NAME fuzz_tester_run COMMAND ccia_fuzz_tests -max_total_time=${FUZZ_RUNTIME} )
  endif()
endif()

# =============================================================================
# Coverage Tests
# =============================================================================

option(ccia_BUILD_GCOV_TESTS "Build coverage test target" ${PROJECT_IS_TOP_LEVEL} # Problems with LLVM:
       # undefined reference to llvm_* functions
)

if(ccia_BUILD_GCOV_TESTS
   AND NOT MSVC
   AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
   AND CMAKE_BUILD_TYPE STREQUAL "Debug"
)
  # ---------------------------------------------------------------------------
  # TARGET: ccia_covr_tests
  # ---------------------------------------------------------------------------
  add_executable(ccia_covr_tests)
  file(GLOB_RECURSE ccia_covr_tests_SOURCE_FILES "tests/UnitTests/*.cpp")
  target_sources(ccia_covr_tests PRIVATE ${ccia_SOURCE_FILES} ${ccia_HEADER_FILES} ${ccia_PUBLIC_HEADER_FILES} ${ccia_covr_tests_SOURCE_FILES})
  target_link_libraries(ccia_covr_tests PRIVATE ccia GTest::gtest GTest::gmock gcov)
  target_include_directories(ccia_covr_tests PRIVATE ${ROOT_DIR}/lib)

  # add pre-compiled-headers
  if(OS_NAME_RESULT STREQUAL "Windows")
    target_precompile_headers(
      ccia_covr_tests
      PRIVATE
      # Windows API
      ${windows_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  elseif(OS_NAME_RESULT STREQUAL "Linux")
    target_precompile_headers(
      ccia_covr_tests
      PRIVATE
      # Linux API
      ${linux_api_library_pch}
      # C++ Standard Library
      ${cpp_standard_library_pch}
      # C Standard Library
      ${c_standard_library_pch}
    )
  else()
    log_fatal("[BUG] OS_NAME_RESULT: ${OS_NAME_RESULT}")
  endif()

  # ---------------------------------------------------------------------------
  # Utilities from CodeCoverage.cmake (in kksk_cmake_modules)
  # ---------------------------------------------------------------------------
  # 1. Append necessary compiler flags for all supported source files
  append_coverage_compiler_flags_to_target(ccia_covr_tests)
  # 1. create a custom make target which runs your test executable and produces a code coverage report
  setup_target_for_coverage_lcov(
    NAME
    ccia_covr_tests_coverage
    EXECUTABLE
    ccia_covr_tests
    DEPENDENCIES
    ccia_covr_tests
    BASE_DIRECTORY
    "${ROOT_DIR}/tests/CovrTests"
    NO_DEMANGLE
  )
  add_custom_command(
    TARGET ccia_covr_tests_coverage
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ROOT_DIR}/logs/ccia_covr_tests_coverage
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/ccia_covr_tests_coverage ${ROOT_DIR}/logs/ccia_covr_tests_coverage
    COMMENT "Copying report to ${ROOT_DIR}/logs/ccia_covr_tests_coverage"
  )
endif()

# ############################################################################
# Installing
# ############################################################################

# ############################################################################
# Packaging
# ############################################################################
