enable_testing()

# Google Testの設定
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# テストの追加
add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test PRIVATE
    toposXi_lib
    gtest_main
)

add_executable(parser_test parser_test.cpp)
target_link_libraries(parser_test PRIVATE
    toposXi_lib
    gtest_main
)

add_executable(type_system_test type_system_test.cpp)
target_link_libraries(type_system_test PRIVATE
    toposXi_lib
    gtest_main
)

# テストの登録
add_test(NAME LexerTest COMMAND lexer_test)
add_test(NAME ParserTest COMMAND parser_test)
add_test(NAME TypeSystemTest COMMAND type_system_test)
