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

add_library(fxt INTERFACE)
target_include_directories(fxt INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
add_library(fxt::fxt ALIAS fxt)

add_executable(fxt_test demo/main.cpp)
target_link_libraries(fxt_test PRIVATE fxt::fxt)