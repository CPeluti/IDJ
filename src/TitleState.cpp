#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"
#include "TitleState.h"
#include "SpriteRenderer.h"
#include "StageState.h"
#include "InputManager.h"
#include "Game.h"
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