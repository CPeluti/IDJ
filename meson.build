project('jogo', 'cpp')
sources = files()

subdir('src')

incdir = include_directories('include')

sdl = 'C:/libs/SDL3-3.2.10/cmake'

sdl3_dep = dependency('SDL3',cmake_module_path:sdl, required: true)
sdl3_image_dep = dependency('SDL3_image', cmake_module_path:sdl, required: true)
sdl3_mixer_dep = dependency('SDL3_mixer', cmake_module_path:sdl, required: true)
sdl3_ttf_dep = dependency('SDL3_ttf', cmake_module_path:sdl, required: true)

executable( 'jogo',
            sources, 
            include_directories : incdir,
            dependencies: [
                sdl3_dep,
                sdl3_image_dep,
                sdl3_mixer_dep,
                sdl3_ttf_dep
            ]
)