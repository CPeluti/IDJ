#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"
#include "State.h"
#include "TileSet.h"
#include "TileMap.h"
#include "Zombie.h"
#include "Character.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "WaveSpawner.h"
#include "Collision.cpp"
#include "Collider.h"
#include <iostream>
#include <set>
#include <algorithm>

#include "Camera.h"

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

State::State() : started(false), quitRequested(false), objectArray()
{
    // // bg = new Sprite();
    // GameObject *start = new GameObject();
    // SpriteRenderer *sr = new SpriteRenderer(*start, "resources/img/Background.png", 1, 1);
    // start->z=-1;
    // sr->SetCameraFollower(true);
    // start->AddComponent(sr);
    // this->AddObject(start);

    GameObject *bg = new GameObject();
    bg->z = 0;
    TileSet *tileset = new TileSet(64, 64, "resources/img/Tileset.png");
    TileMap *tilemap = new TileMap(*bg, "resources/map/map.txt", tileset);
    bg->box.RawMove({-500, -500});
    bg->AddComponent(tilemap);
    this->AddObject(bg);

    music = new Music("resources/audio/BGM.wav");
    music->Play();

    GameObject *character = new GameObject();
    Character *characterComponent = new Character(*character, "resources/img/Player.png", true);
    character->AddComponent(characterComponent);
    this->AddObject(character);
    character->box.RawMove({1280, 1280});
    Character::player = characterComponent;
    Camera::Follow(character);

    GameObject *waveSpawner = new GameObject();
    WaveSpawner *ws = new WaveSpawner(*waveSpawner);
    waveSpawner->AddComponent(ws);
    this->AddObject(waveSpawner);

    // createZombie({1200, 1200}, this);
}
State::~State()
{
    this->objectArray.clear();
}
bool State::QuitRequested()
{
    return quitRequested;
}
void State::LoadAssets() {}
void State::Update(float dt)
{
    InputManager &ip = InputManager::GetInstance();

    Camera::Update(dt);

    if (ip.KeyPress(ESCAPE_KEY) || ip.QuitRequested())
    {
        quitRequested = true;
    }
    if (ip.KeyPress(SPACE_KEY))
    {
        std::cout << "criou zombie" << std::endl;
        // createZombie({ip.GetMouseX(), ip.GetMouseY()}, this);
    }
    for (int i = 0; i < (int)this->objectArray.size(); i++)
    {
        objectArray[i]->Update(dt);
    }
    std::set<std::pair<int, int>> checked;
    for (int i = 0; i < (int)this->objectArray.size(); i++)
    {
        Collider *colliderA = (Collider *)objectArray[i]->GetComponent("Collider");
        if (colliderA != nullptr)
        {
            for (int j = 0; j < (int)this->objectArray.size(); j++)
            {
                if (j != i && checked.find({i, j}) == checked.end() && checked.find({j, i}) == checked.end())
                {
                    checked.insert({i, j});
                    Collider *colliderB = (Collider *)objectArray[j]->GetComponent("Collider");
                    if (colliderB != nullptr)
                    {
                        if (Collision::IsColliding(colliderA->box, colliderB->box, objectArray[i]->angleDeg, objectArray[j]->angleDeg))
                        {
                            objectArray[i]->NotifyCollision(*objectArray[j]);
                            objectArray[j]->NotifyCollision(*objectArray[i]);
                        }
                    }
                }
            }
        }
    }
    if (SDL_QuitRequested())
    {
        quitRequested = true;
    }
    for (int i = 0; i < (int)this->objectArray.size(); i++)
    {
        if (objectArray[i]->IsDead())
        {
            objectArray.erase(objectArray.begin() + i);
        }
    }
}
void State::Render()
{
    std::stable_sort(objectArray.begin(), objectArray.end(), y_sort);
    std::stable_sort(objectArray.begin(), objectArray.end(), z_sort);
    for (int i = 0; i < (int)this->objectArray.size(); i++)
    {
        // std::cout << "rendering" << std::endl;5
        objectArray[i]->Render();
    }
    // bg->Open("resources/img/Background.png");
    // bg->Render(0, 0);
}

void State::Start()
{
    LoadAssets();
    for (unsigned int i = 0; i < objectArray.size(); i++)
    {
        objectArray[i]->Start();
    }
    started = true;
}

std::weak_ptr<GameObject> State::AddObject(GameObject *object)
{
    objectArray.emplace_back(object);
    if (started)
    {
        object->Start();
    }
    return GetObjectPtr(object);
}

std::weak_ptr<GameObject> State::GetObjectPtr(GameObject *object)
{
    for (unsigned int i = 0; i < objectArray.size(); i++)
    {
        if (objectArray[i].get() == object)
        {
            return std::weak_ptr<GameObject>(objectArray[i]);
        }
    }
    return std::weak_ptr<GameObject>();
}