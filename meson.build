project('jogo', 'cpp')

cmake = import('cmake')

buildtype = get_option('buildtype')
if buildtype == 'debug'
  add_project_arguments([ '-DDEBUG' ], language: 'cpp')
endif

install_directory = meson.project_source_root()+'/bin'

includes = [include_directories('include')]
spdlog = include_directories('vendor/spdlog/include')
includes += spdlog

sdl2 = dependency('SDL2', method : 'cmake', modules : ['SDL2::SDL2', 'SDL2::SDL2main'])
sdl2_image = dependency('SDL2_image')
sdl2_mixer = dependency('SDL2_mixer') 
sdl2_ttf = dependency('SDL2_ttf')
sdl2_gpu = dependency('SDL2_gpu')

deps = [sdl2, sdl2_image, sdl2_mixer, sdl2_ttf, sdl2_gpu]

subdir('src')

executable('JOGO', 'src/main.cpp', include_directories: includes, link_with : [core_lib, game_lib], dependencies : deps, install_dir: install_directory, install : true)

install_data(['lib/SDL2.dll','lib/SDL2_image.dll','lib/SDL2_mixer.dll','lib/SDL2_ttf.dll'], install_dir: install_directory)
install_subdir('resources', install_dir: install_directory)