#
# Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 2.8)
project(match3 CXX)

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])*")
  set(CMAKE_CXX_COMPILER_ID "Emscripten")
endif()

add_custom_target(app DEPENDS ${PROJECT_NAME} copy-data-files)
add_custom_target(style)
add_custom_target(copy-data-files ALL COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)
add_custom_command(TARGET style COMMAND find ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/test -iname "*.hpp" -or -iname "*.cpp" | xargs clang-format -i)

find_program(MEMORYCHECK_COMMAND valgrind)
if(MEMORYCHECK_COMMAND-NOTFOUND)
  message("[W] Valgrind not found")
else()
  set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c99-extensions")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Emscripten")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --emrun")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file data --use-preload-plugins")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --shell-file ${CMAKE_SOURCE_DIR}/data/template.html")

  set(WEB "index.html")
  add_custom_target(web DEPENDS ${WEB} copy-data-files)
  add_custom_target(emrun)
  add_custom_command(TARGET emrun COMMAND emrun --port 8080 index.html)
  add_executable(${WEB} src/main.cpp)
else()
  include(FindPkgConfig)
  PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
  PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
  PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
endif()

include_directories(include)
include_directories(libs/di/include)
include_directories(libs/msm-lite/include)
include_directories(libs/range-v3/include)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR})

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})

enable_testing()
add_subdirectory(test)
