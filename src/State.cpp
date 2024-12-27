#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"
#include "State.h"
#include "TileSet.h"
#include "TileMap.h"
#include "Zombie.h"
#include "Character.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
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

State::State() : started (false), quitRequested(false), objectArray()
{
    // bg = new Sprite();
    GameObject *start = new GameObject();
    SpriteRenderer *sr = new SpriteRenderer(*start, "resources/img/Background.png", 1, 1);
    sr->SetCameraFollower(true);
    start->AddComponent(sr);
    this->AddObject(start);

    GameObject* bg = new GameObject();
    TileSet* tileset = new TileSet(64,64,"resources/img/Tileset.png");
    TileMap* tilemap = new TileMap(*bg,"resources/map/map.txt", tileset);
    bg->box.x = -500;
    bg->box.y = -500;
    bg->AddComponent(tilemap);
    this->AddObject(bg);

    music = new Music("resources/audio/BGM.wav");
    music->Play();

    GameObject* character = new GameObject();
    Character* characterComponent = new Character(*character, "resources/img/Player.png");
    character->AddComponent(characterComponent);
    this->AddObject(character);
    character->box.x = 1280;
    character->box.y = 1280;
    Camera::Follow(character);
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
        // std::cout << "rendering" << std::endl;5
        objectArray[i]->Render();
    }
    // bg->Open("resources/img/Background.png");
    // bg->Render(0, 0);
}

void State::Start(){
    LoadAssets();
    for(auto object : objectArray){
        object->Start();
    }
    started = true;
}

std::weak_ptr<GameObject> State::AddObject(GameObject *object)
{
    objectArray.emplace_back(object);
    if(started){
        object->Start();
    }
    return GetObjectPtr(object);
}

std::weak_ptr<GameObject> State::GetObjectPtr(GameObject *object){
    for(int i = 0; i<objectArray.size(); i++){
        if(objectArray[i].get() == object){
            return std::weak_ptr<GameObject>(objectArray[i]);
        }
    }
    return std::weak_ptr<GameObject>();
}