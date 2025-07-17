#pragma once
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
	menu = std::make_shared<Menu>();
    std::shared_ptr<GameObject> fps = std::make_shared<GameObject>();
	std::shared_ptr<Text> fpsText = std::make_shared<Text>(*fps, "resources/font/neodgm.ttf", 10, Text::SOLID, "FPS: ", SDL_Color{ 255, 255, 255 }, 0, true);
    fps->box.RawMove({ 50,50 });
	fps->AddComponent(fpsText);
	this->AddObject(fps);
	this->fpsText = fpsText;

    std::shared_ptr<GameObject> bg = std::make_shared<GameObject>();
    bg->z = 0;
    // TileSet *tileset = new TileSet(16, 16, "resources/img/TilesetCastle.png");
    // TileMap *tilemap = new TileMap(*bg, "resources/map/mapCastle.txt", tileset);
    bg->box.RawMove({0, 0});
    Rect fullTileCollider = {0, 0, 16, 16};
    Rect halfTileLeftCollider = {0, 0, 9, 16};
    Rect halfTileRightCollider = {7, 0, 9, 16};
    TileSet *tileset = new TileSet(16, 16, "resources/img/TilesetCastle.png", std::map<int, Rect>{
        {97, fullTileCollider},
        {41, fullTileCollider},
        {32, fullTileCollider},
        {27, fullTileCollider},
        {29, fullTileCollider},
        {98, fullTileCollider},
        {80, fullTileCollider},
        { 152, fullTileCollider },
        { 151, fullTileCollider },
        { 116, halfTileRightCollider },
        { 134, fullTileCollider },
        { 5, fullTileCollider },
        { 7, fullTileCollider },
        { 8, fullTileCollider },
        { 115, halfTileLeftCollider },
        {116, fullTileCollider},
        {79, fullTileCollider}
    });
    std::shared_ptr<TileMap> tilemap = std::make_shared<TileMap>(*bg, "resources/map/testec.txt", tileset);
    bg->AddComponent(tilemap);
    this->AddObject(bg);

    std::shared_ptr<GameObject> character = std::make_shared<GameObject>();
    std::shared_ptr<Character> characterComponent = std::make_shared<Character>(*character, "resources/img/Protagonista.png", tilemap, true);
    character->AddComponent(characterComponent);
    this->AddObject(character);
    character->box.RawMove({30, 80});
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
    //backgroundMusic = Music("resources/audio/BGM.wav");
}
void StageState::Update(float dt)
{
    InputManager& ip = InputManager::GetInstance();
    if (SDL_QuitRequested() || ip.QuitRequested())
    {
        quitRequested = true;
    }
    if (m_paused) {
		menu->Update(dt);
        return;
    };
    float fps = Game::GetInstance().GetFps();
    std::string fpsstring = fmt::format("FPS: {}", fps);
    if(auto shared = fpsText.lock())
    {
        shared->SetText(fpsstring);
	}

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
    menu->Render();
    // bg->Open("resources/img/Background.png");
    // bg->Render(0, 0);
}

void StageState::Start()
{
    menu->Start();
    LoadAssets();
    StartArray();
    started = true;
    //backgroundMusic.Play();
}

void StageState::Resume() { this->m_paused = false; menu->enabled = false;}
void StageState::Pause() { this->m_paused = true; menu->enabled = true;}