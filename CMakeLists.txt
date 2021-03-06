cmake_minimum_required(VERSION 3.0.0)
project(sudoku_solver VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(SOURCES
    src/process_args.cpp
    src/backtrack.cpp
    src/candidates.cpp
    src/logic.cpp
    src/main.cpp
    src/print.cpp
    src/puzzle.cpp
    src/symbol.cpp
    src/util.cpp
    include/process_args.hpp
    include/colors.hpp
    include/print.hpp
    include/puzzle.hpp
    include/symbol.hpp
    include/util.hpp
)

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

add_executable(sudoku_solver ${SOURCES})

target_include_directories(sudoku_solver PRIVATE ${PROJECT_SOURCE_DIR}/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(sudoku_solver PRIVATE Threads::Threads)




