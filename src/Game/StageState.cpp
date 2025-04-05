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

#include "../Core/Collision.cpp"


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
    GameObject *enemy = new GameObject();
    Component *zombie = new Zombie(*enemy);
    enemy->AddComponent(zombie);
    state->AddObject(enemy);

    enemy->box.Move(pos);
}

StageState::StageState() : backgroundMusic("resources/audio/BGM.wav")
{
    GameObject *bg = new GameObject();
    bg->z = 0;
    TileSet *tileset = new TileSet(64, 64, "resources/img/Tileset.png");
    TileMap *tilemap = new TileMap(*bg, "resources/map/map.txt", tileset);
    bg->box.RawMove({0, 0});
    bg->AddComponent(tilemap);
    this->AddObject(bg);

    GameObject *character = new GameObject();
    Character *characterComponent = new Character(*character, "resources/img/Player.png", true);
    character->AddComponent(characterComponent);
    player = this->AddObject(character);
    character->box.RawMove({1280, 1280});
    Character::player = characterComponent;
    Camera::Follow(character);

    GameObject *waveSpawner = new GameObject();
    WaveSpawner *ws = new WaveSpawner(*waveSpawner);
    waveSpawner->AddComponent(ws);
    spawner = this->AddObject(waveSpawner);

}
StageState::~StageState()
{
    this->objectArray.clear();
}
void StageState::LoadAssets() {
    backgroundMusic = Music("resources/audio/BGM.wav");

}
void StageState::Update(float dt)
{
    InputManager &ip = InputManager::GetInstance();

    Camera::Update(dt);

    if (ip.KeyPress(ESCAPE_KEY))
    {
        popRequested = true;
        TitleState* stage = new TitleState();
        Game::GetInstance().Push(stage);
    }
    UpdateArray(dt);
    if(auto p = player.lock()){
        if(p->IsDead()){
            popRequested = true;
            GameData::playerWon = false;
            EndState* stage = new EndState();
            Game::GetInstance().Push(stage);
            backgroundMusic.Stop();
            return;
        }
        if(auto s = spawner.lock()){
            if(s->IsDead() && !p->IsDead()){
                popRequested = true;
                GameData::playerWon = true;
                EndState* stage = new EndState();
                backgroundMusic.Stop();
                Game::GetInstance().Push(stage);
                return;
            }
        }
    }
    std::set<std::pair<int, int>> checked;
    for (int i = 0; i < (int)this->objectArray.size(); i++)
    {
        Collider *colliderA = (Collider *)objectArray[i]->GetComponent("Collider");
        if (colliderA != nullptr)
        {
            for (int j = i+1; j < (int)this->objectArray.size(); j++)
            {
                Collider *colliderB = (Collider *)objectArray[j]->GetComponent("Collider");
                if (colliderB != nullptr)
                {
                    if (Collision::IsColliding(colliderA->box, colliderB->box, objectArray[i]->angleDeg, objectArray[j]->angleDeg))
                    {
                        OnCollisionEvent a(*objectArray[i]);
                        OnCollisionEvent b(*objectArray[j]);
                        objectArray[i]->subject.notify(b);
                        objectArray[j]->subject.notify(a);
                    }
                }
            }
        }
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
        } else {
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

void StageState::Resume(){}
void StageState::Pause(){}