cmake_minimum_required(VERSION 3.10)
project(CharConverter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-fno-exceptions" COMPILER_SUPPORTS_FNO_EXCEPTIONS)
check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)
check_cxx_compiler_flag("-fno-rtti" COMPILER_SUPPORTS_FNO_RTTI)

if(COMPILER_SUPPORTS_FNO_EXCEPTIONS)
  add_compile_options(-fno-exceptions)
endif()
if(COMPILER_SUPPORTS_WALL)
  add_compile_options(-Wall)
endif()
if(COMPILER_SUPPORTS_FNO_RTTI)
  add_compile_options(-fno-rtti)
endif()

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

add_executable(CharConverter
  "${SRC_DIR}/main.cpp"
  "${SRC_DIR}/UTF8ToUTF32.cpp"
)

target_include_directories(CharConverter PRIVATE
"${SRC_DIR}/include"
)
