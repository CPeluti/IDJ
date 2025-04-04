cmake_minimum_required(VERSION 4.0)
set( CMAKE_PREFIX_PATH "C:/libs/SDL3-3.2.10" )
set( CMAKE_INSTALL_PREFIX "C:/Users/peluti/projetos/IDJ" )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_COMPILER "C:\\msys64\\ucrt64\\bin\\g++.exe")
# set(SDL3_GPU_LIBRARY_PATH "C:/SDL3-2.32.2/lib/libSDL3_gpu.dll.a")
# set(SDL3_GPU_INCLUDE_PATH "C:/Development/Library/include/SDL3_gpu")
project(JOGO)


option(MYGAME_VENDORED "Use vendored libraries" OFF)
# list(APPEND CMAKE_PREFIX_PATH "C:/setupsdl")

# file(GLOB HEADERS "./include/*.h")
# file(GLOB libs "./include/^(?!main).*.cpp")


# target_include_directories(JOGO PRIVATE ./include)

include_directories(include)
# add_subdirectory(include)
file(GLOB SOURCES "src/*/*.cpp")


if(NOT TARGET spdlog)
    # Stand-alone build
    add_subdirectory(vendor/spdlog)
endif()
if(MYGAME_VENDORED)
    add_subdirectory(vendor/sdl EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
    find_package(SDL3_image REQUIRED CONFIG REQUIRED COMPONENTS SDL3_image)
    find_package(SDL3_mixer REQUIRED)
    find_package(SDL3_ttf REQUIRED CONFIG REQUIRED COMPONENTS SDL3_ttf)

    # 1. Look for a SDL3 package, 2. Look for the SDL3maincomponent and 3. DO NOT fail when SDL3main is not available
endif()

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# SDL3::SDL3main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL3::SDL3main)
    # It has an implicit dependency on SDL3 functions, so it MUST be added before SDL3::SDL3 (or SDL3::SDL3-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3main)
endif()

if(TARGET SDL3_image::SDL3_image)
    # It has an implicit dependency on SDL3 functions, so it MUST be added before SDL3::SDL3 (or SDL3::SDL3-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)
endif()


if(TARGET SDL3_mixer::SDL3_mixer)
    # It has an implicit dependency on SDL3 functions, so it MUST be added before SDL3::SDL3 (or SDL3::SDL3-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_mixer::SDL3_mixer)
endif()

if(TARGET SDL3_ttf::SDL3_ttf)
    # It has an implicit dependency on SDL3 functions, so it MUST be added before SDL3::SDL3 (or SDL3::SDL3-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)
endif()

# Link to the actual SDL3 library. SDL3::SDL3 is the shared SDL library, SDL3::SDL3-static is the static SDL libarary.
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
install(TARGETS ${PROJECT_NAME})
install(DIRECTORY resources DESTINATION bin)
install(
    FILES
    lib/SDL3.dll
    lib/SDL3_image.dll
    lib/SDL3_mixer.dll
    lib/SDL3_ttf.dll
    DESTINATION
    bin
)
