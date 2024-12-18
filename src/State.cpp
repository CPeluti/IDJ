#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"
#include "State.h"
#include "TileSet.h"
#include "TileMap.h"
#include "Zombie.h"
#include "InputManager.h"
#include <iostream>

#include "Camera.h"

GameObject* createZombie (int x, int y, State* state){
    GameObject* enemy = new GameObject();
    Component *zombie = new Zombie(*enemy);
    enemy->AddComponent(zombie);
    state->AddObject(enemy);
    enemy->box.x = x-enemy->box.w/2; 
    enemy->box.y = y-enemy->box.h/2; 
    return enemy;
}

State::State()
{
    quitRequested = false;
    // bg = new Sprite();
    // GameObject *bg = new GameObject();
    // SpriteRenderer *sr = new SpriteRenderer(*bg, "resources/img/Background.png", 1, 1);
    // bg->AddComponent(sr);
    

    GameObject* bg = new GameObject();
    TileSet* tileset = new TileSet(64,64,"resources/img/Tileset.png");
    TileMap* tilemap = new TileMap(*bg,"resources/map/map.txt", tileset);
    bg->box.x = -500;
    bg->box.y = -500;
    bg->AddComponent(tilemap);
    this->AddObject(bg);

    music = new Music("resources/audio/BGM.wav");
    music->Play();
    GameObject* enemy = createZombie(500, 400, this);

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
    InputManager& ip = InputManager::GetInstance();

    Camera::Update(dt);
    
    if(ip.KeyPress(ESCAPE_KEY) || ip.QuitRequested()){
        quitRequested = true;
    }
    if(ip.KeyPress(SPACE_KEY)){
        createZombie(ip.GetMouseX(), ip.GetMouseY(), this);
    }
    for (int i = 0; i < (int)this->objectArray.size(); i++)
    {
        objectArray[i]->Update(dt);
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
    for (int i = 0; i < (int)this->objectArray.size(); i++)
    {
        // std::cout << "rendering" << std::endl;
        objectArray[i]->Render();
    }
    // bg->Open("resources/img/Background.png");
    // bg->Render(0, 0);
}
void State::AddObject(GameObject *object)
{
    objectArray.emplace_back(std::unique_ptr<GameObject>(object));
}
