cmake_minimum_required(VERSION 3.16)
project(tetris_c11 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(TETRIS_BUILD_TESTS "Build unit tests" ON)

# Warnings (corporate-grade)
if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(
    -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wstrict-prototypes
    -Wmissing-prototypes -Wno-unused-parameter
  )
endif()

# Default build type
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Logging: disable DEBUG in Release
add_compile_definitions($<$<CONFIG:Release>:TETRIS_NO_DEBUG=1>)

find_package(SDL2 REQUIRED)

include_directories(
  ${SDL2_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(TETRIS_SOURCES
  src/utils/log.c
  src/utils/str.c
  src/utils/config.c

  src/core/time.c
  src/core/event_bus.c
  src/core/state.c
  src/core/app.c

  src/input/input.c

  src/render/font5x7.c
  src/render/renderer.c

  src/gameplay/board.c
  src/gameplay/piece.c
  src/gameplay/srs.c
  src/gameplay/randomizer.c
  src/gameplay/scoring.c
  src/gameplay/gameplay.c

  src/ui/fx.c
  src/ui/hud.c
  src/ui/state_menu.c
  src/ui/state_options.c
  src/ui/state_controls.c
  src/ui/state_credits.c
  src/ui/state_play.c
  src/ui/state_pause.c
  src/ui/state_gameover.c

  src/main.c
)

add_executable(tetris ${TETRIS_SOURCES})
target_link_libraries(tetris ${SDL2_LIBRARIES})

# Tests: pure gameplay logic (no SDL dependency)
if (TETRIS_BUILD_TESTS)
  enable_testing()

  set(TETRIS_TEST_SOURCES
    # utils (podem ser dependência indireta do event_bus/log)
    src/utils/log.c
    src/utils/str.c
    src/utils/config.c

    # core (necessário porque gameplay publica eventos)
    src/core/event_bus.c

    # gameplay
    src/gameplay/board.c
    src/gameplay/piece.c
    src/gameplay/srs.c
    src/gameplay/randomizer.c
    src/gameplay/scoring.c
    src/gameplay/gameplay.c

    # tests
    tests/test_main.c
    tests/test_board.c
    tests/test_srs.c
    tests/test_randomizer.c
    tests/test_scoring.c
  )

  add_executable(tetris_tests ${TETRIS_TEST_SOURCES})
  target_include_directories(tetris_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  add_test(NAME tetris_tests COMMAND tetris_tests)
endif()
