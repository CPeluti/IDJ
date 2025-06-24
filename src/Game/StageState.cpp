#include <iostream>
#include <set>
#include <algorithm>

#define INCLUDE_SDL_IMAGE
#include "Core/SDL_include.h"
#include "Core/TileMap.h"
#include "Core/InputManager.h"
#include "Core/SpriteRenderer.h"
#include "Core/Game.h"
#include "Core/Collider.h"
#include "Core/Camera.h"
#include "Core/Log.h"

#include "Core/Collision.h"

#include "Game/StageState.h"
#include "Game/TitleState.h"
#include "Game/Zombie.h"
#include "Game/Character.h"
#include "Game/EndState.h"
#include "Game/GameData.h"

bool y_sort(std::shared_ptr<GameObject> i, std::shared_ptr<GameObject> j)
{
    return (i->box.GetPos().y < j->box.GetPos().y);
}

bool z_sort(std::shared_ptr<GameObject> i, std::shared_ptr<GameObject> j)
{
    return (i->z < j->z);
}

void createZombie(Vec2 pos, State *state)
{
    std::shared_ptr<GameObject> enemy = std::make_shared<GameObject>();
    std::shared_ptr<Zombie> zombie = std::make_shared<Zombie>(*enemy);
    enemy->AddComponent(zombie);
    state->AddObject(enemy);

    enemy->box.Move(pos);
}

StageState::StageState() : backgroundMusic("resources/audio/BGM.wav")
{
    Camera::zoom = 3.0f;

    std::shared_ptr<GameObject> bg = std::make_shared<GameObject>();
    bg->z = 0;
    // TileSet *tileset = new TileSet(16, 16, "resources/img/TilesetCastle.png");
    // TileMap *tilemap = new TileMap(*bg, "resources/map/mapCastle.txt", tileset);
    bg->box.RawMove({0, 0});
    Rect r = {0, 0, 16, 16};
    TileSet *tileset = new TileSet(16, 16, "resources/img/TilesetCastle.png", std::map<int, Rect>{{4, r}, {14, r}, {22, r}, {51, r}, {69, r}, {40, r}, {24, r}, {68, r}, {50, r}, {24, r}});
    std::shared_ptr<TileMap> tilemap = std::make_shared<TileMap>(*bg, "resources/map/mapCastle.txt", tileset);
    bg->AddComponent(tilemap);
    this->AddObject(bg);

    std::shared_ptr<GameObject> character = std::make_shared<GameObject>();
    std::shared_ptr<Character> characterComponent = std::make_shared<Character>(*character, "resources/img/Player.png", true);
    character->AddComponent(characterComponent);
    this->AddObject(character);
    character->box.RawMove({0, 0});
    Character::player = characterComponent;
    Camera::Follow(character);
    // std::shared_ptr<GameObject> waveSpawner = std::make_shared<GameObject>();
    // std::shared_ptr<WaveSpawner>ws = std::make_shared<WaveSpawner>(*waveSpawner);
    // waveSpawner->AddComponent(ws);
    // spawner = this->AddObject(waveSpawner);
}
StageState::~StageState()
{
    SDL_StopTextInput();
    this->objectArray.clear();
}
void StageState::LoadAssets()
{
    backgroundMusic = Music("resources/audio/BGM.wav");
}
void StageState::Update(float dt)
{
    InputManager &ip = InputManager::GetInstance();

    Camera::Update(dt);

    UpdateArray(dt);
    if (auto p = player.lock())
    {
        if (p->IsDead())
        {
            popRequested = true;
            GameData::playerWon = false;
            Game::GetInstance().Push(std::make_unique<EndState>());
            backgroundMusic.Stop();
            return;
        }
        if (auto s = spawner.lock())
        {
            if (s->IsDead() && !p->IsDead())
            {
                popRequested = true;
                GameData::playerWon = true;
                backgroundMusic.Stop();
                Game::GetInstance().Push(std::make_unique<EndState>());
                return;
            }
        }
    }
    this->checkCollisions(dt);
    TypingSystem &ts = TypingSystem::GetInstance();

    ts.Update(dt);


    if (ts.IsTypingMode() && ts.HasSubmitted())
    {
        std::string finalText = ts.GetText();

        ts.CleanText();
        ts.ResetSubmission();
    }

    if (SDL_QuitRequested() || ip.QuitRequested())
    {
        quitRequested = true;
    }
    for (int i = 0; i < (int)this->objectArray.size();)
    {
        if (objectArray[i]->IsDead())
        {
            objectArray.erase(objectArray.begin() + i);
        }
        else
        {
            i++;
        }
    }
}
void StageState::Render()
{
    std::stable_sort(objectArray.begin(), objectArray.end(), y_sort);
    std::stable_sort(objectArray.begin(), objectArray.end(), z_sort);
    RenderArray();
    // bg->Open("resources/img/Background.png");
    // bg->Render(0, 0);
}

void StageState::Start()
{
    LoadAssets();
    StartArray();
    started = true;
    backgroundMusic.Play();
}

void StageState::Resume() {}
void StageState::Pause() {}