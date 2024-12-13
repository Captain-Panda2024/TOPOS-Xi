cmake_minimum_required(VERSION 3.20)
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

# コード生成オプション
option(BUILD_TESTING "Build tests" ON)
option(CODE_COVERAGE "Enable coverage reporting" ON)

# コードカバレッジの設定
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
endif()

# ソースファイルの設定
set(COMPILER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/math/math_topology_verifier.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/topology/type_topology_verifier.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/topology/type_topology_verifier.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/parser/parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compiler/ast/ast.cpp"
)

file(GLOB_RECURSE RUNTIME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/runtime/*.cpp"
)

set(SOURCES
    ${COMPILER_SOURCES}
    ${RUNTIME_SOURCES}
)

# ライブラリターゲットの作成
add_library(toposXi_lib STATIC ${SOURCES})
target_include_directories(toposXi_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler
)

# LLVMライブラリのリンク
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(toposXi_lib PUBLIC ${llvm_libs})

# テストの設定
add_subdirectory(tests)

# インストールルール
install(TARGETS toposXi_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

# パッケージ設定
set(CPACK_PACKAGE_NAME "toposXi")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TOPOS-Ξ Quantum Compiler")
include(CPack)
