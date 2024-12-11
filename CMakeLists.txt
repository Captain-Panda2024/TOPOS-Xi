cmake_minimum_required(VERSION 3.15)
project(toposXi VERSION 0.1.0 LANGUAGES CXX)

# C++17を要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# コンパイラ警告の設定
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# LLVMの依存関係
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# ヘッダーファイルの場所を指定
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# ソースファイルの収集
file(GLOB_RECURSE SOURCES 
    "src/compiler/*.cpp"
    "src/runtime/*.cpp"
)

# ライブラリターゲットの作成
add_library(toposXi_lib STATIC ${SOURCES})
target_include_directories(toposXi_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 実行可能ファイルの作成
add_executable(toposXi src/main.cpp)
target_link_libraries(toposXi PRIVATE toposXi_lib)

# テストの設定
enable_testing()
add_subdirectory(tests)
