cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(bastet VERSION 1.0.0)

add_executable(bastet src/bastet.cpp)
target_include_directories(bastet PUBLIC ${CHESS_ROOT}/include)
find_library(
  CHESS_LIB
  NAMES chess
  PATHS ${CHESS_ROOT}/lib)
target_link_libraries(bastet PUBLIC ${CHESS_LIB})

execute_process(
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
