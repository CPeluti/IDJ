core_files = [
    'Core/Animation.cpp',
    'Core/Animator.cpp',
    'Core/Camera.cpp',
    'Core/Collider.cpp',
    'Core/Collision.cpp',
    'Core/Component.cpp',
    'Core/Game.cpp',
    'Core/GameObject.cpp',
    'Core/InputManager.cpp',
    'Core/Log.cpp',
    'Core/Music.cpp',
    'Core/Observer.cpp',
    'Core/ParticleSystem.cpp',
    'Core/Random.cpp',
    'Core/Rect.cpp',
    'Core/Resources.cpp',
    'Core/Sound.cpp',
    'Core/Sprite.cpp',
    'Core/SpriteRenderer.cpp',
    'Core/State.cpp',
    'Core/Subject.cpp',
    'Core/Text.cpp',
    'Core/TileMap.cpp',
    'Core/TileSet.cpp',
    'Core/Vec2.cpp',

]

game_files = [
    'Game/AIController.cpp',
    'Game/Bullet.cpp',
    'Game/Character.cpp',
    'Game/EndState.cpp',
    'Game/Entity.cpp',
    'Game/GameData.cpp',
    'Game/Gun.cpp',
    'Game/HealthSystem.cpp',
    'Game/Lifebar.cpp',
    'Game/PlayerController.cpp',
    'Game/StageState.cpp',
    'Game/TitleState.cpp',
    'Game/Wave.cpp',
    'Game/WaveSpawner.cpp',
    'Game/Zombie.cpp',
]

core_lib = shared_library('core', sources: core_files, include_directories: includes, dependencies: deps, install: true, install_dir: install_directory)
game_lib = shared_library('game', sources: game_files, include_directories: includes, dependencies: deps, link_with: [core_lib], install: true, install_dir: install_directory)