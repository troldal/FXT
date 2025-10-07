#==============================================================================
# Preamble
#==============================================================================
cmake_minimum_required(VERSION 3.20)
project(FXT)

#==============================================================================
# Set compiler settings
#==============================================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_definitions(NOMINMAX)


#==============================================================================
# Fetch dependency manager
#==============================================================================

# download CPM.cmake
file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

#==============================================================================
# Fetch dependencies
#==============================================================================

CPMFindPackage(
        NAME optional
        GITHUB_REPOSITORY TartanLlama/optional
        VERSION 1.1.0
        EXCLUDE_FROM_ALL
        SYSTEM
)

CPMFindPackage(
        NAME expected
        GITHUB_REPOSITORY TartanLlama/expected
        VERSION 1.1.0
        EXCLUDE_FROM_ALL
        SYSTEM
)




#==============================================================================
# Add targets
#==============================================================================
add_library(fxt INTERFACE)
target_include_directories(fxt INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(fxt INTERFACE tl::expected tl::optional)
add_library(fxt::fxt ALIAS fxt)

if(PROJECT_IS_TOP_LEVEL)
    add_executable(fxt_test demo/main.cpp)
    target_link_libraries(fxt_test PRIVATE fxt::fxt)

    add_executable(fxt_and_then_demo demo/AndThen.cpp)
    target_link_libraries(fxt_and_then_demo PRIVATE fxt::fxt)

    add_executable(fxt_transform_demo demo/Transform.cpp)
    target_link_libraries(fxt_transform_demo PRIVATE fxt::fxt)

    add_executable(fxt_or_else_demo demo/OrElse.cpp)
    target_link_libraries(fxt_or_else_demo PRIVATE fxt::fxt)

    add_executable(fxt_value_demo demo/Value.cpp)
    target_link_libraries(fxt_value_demo PRIVATE fxt::fxt)

    add_executable(fxt_value_or_demo demo/ValueOr.cpp)
    target_link_libraries(fxt_value_or_demo PRIVATE fxt::fxt)

    add_executable(fxt_curry_demo demo/Curry.cpp)
    target_link_libraries(fxt_curry_demo PRIVATE fxt::fxt)

    add_executable(fxt_drop_demo demo/Drop.cpp)
    target_link_libraries(fxt_drop_demo PRIVATE fxt::fxt)

    add_executable(fxt_take_demo demo/Take.cpp)
    target_link_libraries(fxt_take_demo PRIVATE fxt::fxt)

    add_executable(fxt_match_demo demo/Match.cpp)
    target_link_libraries(fxt_match_demo PRIVATE fxt::fxt)

    add_executable(fxt_transform_error_demo demo/TransformError.cpp)
    target_link_libraries(fxt_transform_error_demo PRIVATE fxt::fxt)

    add_executable(fxt_to_optional_demo demo/ToOptional.cpp)
    target_link_libraries(fxt_to_optional_demo PRIVATE fxt::fxt)

    add_executable(fxt_to_expected_demo demo/ToExpected.cpp)
    target_link_libraries(fxt_to_expected_demo PRIVATE fxt::fxt)

    add_executable(fxt_tap_demo demo/Tap.cpp)
    target_link_libraries(fxt_tap_demo PRIVATE fxt::fxt)

    add_executable(fxt_tap_error_and_none_demo demo/TapErrorAndNone.cpp)
    target_link_libraries(fxt_tap_error_and_none_demo PRIVATE fxt::fxt)

    add_executable(fxt_failure_demo demo/Failure.cpp)
    target_link_libraries(fxt_failure_demo PRIVATE fxt::fxt)

    add_executable(fxt_append_demo demo/Append.cpp)
    target_link_libraries(fxt_append_demo PRIVATE fxt::fxt)

    add_executable(fxt_apply_demo demo/Apply.cpp)
    target_link_libraries(fxt_apply_demo PRIVATE fxt::fxt)

    add_executable(fxt_get_demo demo/Get.cpp)
    target_link_libraries(fxt_get_demo PRIVATE fxt::fxt)
endif()

#==============================================================================
# Options
#==============================================================================

option(FXT_USE_TL_EXPECTED "Use TartanLlama's expected library instead of std::expected" OFF)

if(FXT_USE_TL_EXPECTED)
    message(STATUS "Using TartanLlama's expected library")
    target_compile_definitions(fxt INTERFACE FXT_USE_TL_EXPECTED)
else()
    message(STATUS "Using std::expected")
endif()

option(FXT_USE_TL_OPTIONAL "Use TartanLlama's optional library instead of std::optional" OFF)

if(FXT_USE_TL_OPTIONAL)
    message(STATUS "Using TartanLlama's optional library")
    target_compile_definitions(fxt INTERFACE FXT_USE_TL_OPTIONAL)
else()
    message(STATUS "Using std::optional")
endif()

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(tests)
endif()