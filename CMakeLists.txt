cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(bastet VERSION 1.1.0)

set(SUFFIX
    ""
    CACHE STRING "")
if(SUFFIX)
  set(TARGET_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${SUFFIX}")
else()
  set(TARGET_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
endif()

add_executable(${TARGET_NAME} src/bastet.cpp)
target_include_directories(${TARGET_NAME} PUBLIC ${CHESS_ROOT}/include)
find_library(
  CHESS_LIB
  NAMES chess
  PATHS ${CHESS_ROOT}/lib)
target_link_libraries(${TARGET_NAME} PUBLIC ${CHESS_LIB})

execute_process(
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
