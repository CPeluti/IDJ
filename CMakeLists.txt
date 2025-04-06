cmake_minimum_required(VERSION 4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_PREFIX_PATH "C:/libs/SDL2-2.32.2")
set(CMAKE_INSTALL_PREFIX "C:/Users/peluti/projetos/IDJ/bin")
set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe")
project(JOGO)


option(MYGAME_VENDORED "Use vendored libraries" OFF)
# list(APPEND CMAKE_PREFIX_PATH "C:/setupsdl")

# file(GLOB HEADERS "./include/*.h")
# file(GLOB libs "./include/^(?!main).*.cpp")


# target_include_directories(JOGO PRIVATE ./include)

include_directories(include)
# add_subdirectory(include)
add_definitions(${NUKLEAR_CXX_FLAGS})
file(GLOB_RECURSE SOURCES "src/*.cpp")


if(NOT TARGET spdlog)
    # Stand-alone build
    add_subdirectory(vendor/spdlog)
endif()
if(MYGAME_VENDORED)
    add_subdirectory(vendor/sdl EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
    find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image)
    find_package(SDL2_gpu REQUIRED)
    find_package(SDL2_mixer REQUIRED CONFIG REQUIRED COMPONENTS SDL2_mixer)
    find_package(SDL2_ttf REQUIRED CONFIG REQUIRED COMPONENTS SDL2_ttf)

endif()

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()

if(TARGET SDL2_image::SDL2_image)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image::SDL2_image)
endif()


if(TARGET SDL2_mixer::SDL2_mixer)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_mixer::SDL2_mixer)
endif()

if(TARGET SDL2_ttf::SDL2_ttf)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_ttf::SDL2_ttf)
endif()

if(TARGET SDL2_gpu::SDL2_gpu)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_gpu::SDL2_gpu)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
install(TARGETS ${PROJECT_NAME})
install(DIRECTORY resources DESTINATION bin)
install(
    FILES
    lib/SDL2.dll
    lib/SDL2_image.dll
    lib/SDL2_mixer.dll
    lib/SDL2_ttf.dll
    lib/SDL2_gpu.dll
    DESTINATION
    bin
)
