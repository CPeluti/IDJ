project('jogo', 'cpp')

cmake = import('cmake')

buildtype = get_option('buildtype')
if buildtype == 'debug'
  add_project_arguments([ '-DDEBUG' ], language: 'cpp')
endif

install_directory = meson.source_root()+'/bin'

includes = [include_directories('include')]
spdlog = include_directories('vendor/spdlog/include')
includes += spdlog

sdl3 = dependency('SDL3') 
sdl3_image = dependency('SDL3_image')
sdl3_mixer = dependency('SDL3_mixer') 
sdl3_ttf = dependency('SDL3_ttf')

deps = [sdl3, sdl3_image, sdl3_mixer, sdl3_ttf]

subdir('src')

executable('JOGO', 'src/main.cpp', include_directories: includes, link_with : [core_lib, game_lib], dependencies : deps, install_dir: install_directory, install : true)

install_data(['lib/SDL3.dll','lib/SDL3_image.dll','lib/SDL3_mixer.dll','lib/SDL3_ttf.dll'], install_dir: install_directory)
install_subdir('resources', install_dir: install_directory)