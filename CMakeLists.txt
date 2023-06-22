cmake_minimum_required(VERSION 3.20)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

project(isearch)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/)
set(COMMANDS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)

if (EXISTS ${COMMANDS_FILE})
  file(COPY ${COMMANDS_FILE} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/)
endif()

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(GLOB isearch_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lang/*.c
)

add_executable(isearch_e ${isearch_sources})
add_library(isearch_shared SHARED ${isearch_sources})
add_library(isearch_static STATIC ${isearch_sources})

# set(ISEARCH_COMPILE_FLAGS -g -Wall)
set(ISEARCH_COMPILE_FLAGS_DEBUG -g -Wall)
target_compile_options(isearch_e PRIVATE ${ISEARCH_COMPILE_FLAGS_DEBUG})
target_compile_options(isearch_shared PRIVATE ${ISEARCH_COMPILE_FLAGS_DEBUG})
target_compile_options(isearch_static PRIVATE ${ISEARCH_COMPILE_FLAGS_DEBUG})

include(isearch_deps)

isearch_init_deps(isearch_e)
isearch_init_deps(isearch_shared)
isearch_init_deps(isearch_static)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  isearch_e
  PUBLIC
  $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  isearch_shared
  PUBLIC
  $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include)



target_include_directories(
  isearch_static
  PUBLIC
  $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

#add_subdirectory(test)
