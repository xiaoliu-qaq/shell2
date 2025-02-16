find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
bison_target(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
add_flex_bison_dependency(lexer parser)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(ish ish.c command.c ${FLEX_lexer_OUTPUTS} ${BISON_parser_OUTPUTS})
target_link_libraries(ish ${FLEX_LIBRARIES})
