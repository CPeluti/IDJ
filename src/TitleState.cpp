#define INCLUDE_SDL_IMAGE
#include "Core/SDL_include.h"
#include "Core/TitleState.h"
#include "Core/SpriteRenderer.h"
#include "Core/StageState.h"
#include "Core/InputManager.h"
#include "Core/Camera.h"
#include "Core/Game.h"
#include <iostream>
#include <set>
#include <algorithm>

TitleState::TitleState()
{
    GameObject *start = new GameObject();
    SpriteRenderer *sr = new SpriteRenderer(*start, "resources/img/Title.png", 1, 1);
    sr->SetCameraFollower(true);
    start->AddComponent(sr);
    this->AddObject(start);

    GameObject* text = new GameObject();
    Text* textComponent = new Text(*text, "resources/font/neodgm.ttf", 20, Text::SOLID, "Press SPACEBAR to play again or ESC to leave",{255,255,255}, 1);
    text->AddComponent(textComponent);
    this->AddObject(text);
    text->box.Move({Camera::pos.x+Game::GetInstance().GetWindowSize().x/2, Camera::pos.y+Game::GetInstance().GetWindowSize().y/2});
    text->z = 1;
}
TitleState::~TitleState()
{
    this->objectArray.clear();
}
void TitleState::LoadAssets() {}
void TitleState::Update(float dt)
{
    InputManager &ip = InputManager::GetInstance();

    if (ip.KeyPress(ESCAPE_KEY) || ip.QuitRequested() || SDL_QuitRequested())
    {
        quitRequested = true;
    }
    if (ip.KeyPress(SPACE_KEY))
    {
        popRequested = true;
        StageState* stage = new StageState();
        Game::GetInstance().Push(stage);
    }

    UpdateArray(dt);

}
void TitleState::Render()
{
    RenderArray();
}

void TitleState::Start()
{
    LoadAssets();
    StartArray();
    started = true;

}

void TitleState::Resume(){}
void TitleState::Pause(){}