cmake_minimum_required(VERSION 3.31.2)

project(JOGO)

option(MYGAME_VENDORED "Use vendored libraries" OFF)
# list(APPEND CMAKE_PREFIX_PATH "C:/setupsdl")

# file(GLOB HEADERS "./include/*.h")
# file(GLOB libs "./include/^(?!main).*.cpp")


# target_include_directories(JOGO PRIVATE ./include)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")


if(MYGAME_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
    find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image)
    find_package(SDL2_mixer REQUIRED CONFIG REQUIRED COMPONENTS SDL2_mixer)

    # 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
endif()

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(JOGO PRIVATE SDL2::SDL2main)
endif()

if(TARGET SDL2_image::SDL2_image)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(JOGO PRIVATE SDL2_image::SDL2_image)
endif()


if(TARGET SDL2_mixer::SDL2_mixer)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(JOGO PRIVATE SDL2_mixer::SDL2_mixer)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(JOGO PRIVATE SDL2::SDL2)
