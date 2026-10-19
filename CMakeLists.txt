cmake_minimum_required(VERSION 3.24)
project(chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)  # enlace estático: no hay que copiar DLLs
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
FetchContent_Declare(SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-3.2.0   # o la release-3.x más reciente
  GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(SDL3)

# Cada .cpp nuevo del proyecto se añade a esta lista
add_executable(chip8
  src/main.cpp
  src/Chip8.cpp
  src/Platform.cpp
  src/logger.cpp
  src/cmdLineParser.cpp
  src/romLoader.cpp
)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)