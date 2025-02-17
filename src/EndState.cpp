#include "EndState.h"
#include "GameData.h"
#include "Game.h"
#include "Camera.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "InputManager.h"
#include "TitleState.h"

EndState::EndState(){
    if(GameData::playerWon){
        GameObject *start = new GameObject();
        SpriteRenderer *sr = new SpriteRenderer(*start, "resources/img/Win.png", 1, 1);
        sr->SetCameraFollower(true);
        start->AddComponent(sr);
        this->AddObject(start);
        
        backgroundMusic = Music("resources/audio/win.ogg");

    }else {
        GameObject *start = new GameObject();
        SpriteRenderer *sr = new SpriteRenderer(*start, "resources/img/Lose.png", 1, 1);
        sr->SetCameraFollower(true);
        start->AddComponent(sr);
        this->AddObject(start);

        backgroundMusic = Music("resources/audio/lose.ogg");
    }
    GameObject* text = new GameObject();
    Text* textComponent = new Text(*text, "resources/font/neodgm.ttf", 20, Text::SOLID, "Press SPACEBAR to play again or ESC to leave",{255,255,255}, 1);
    text->AddComponent(textComponent);
    this->AddObject(text);
    text->box.Move({Camera::pos.x+Game::GetInstance().GetWindowSize().x/2, Camera::pos.y+Game::GetInstance().GetWindowSize().y/2});
    text->z = 1;
}

EndState::~EndState(){}

void EndState::Start(){
    backgroundMusic.Play();
}

void EndState::Update(float dt){
    InputManager &ip = InputManager::GetInstance();

    if (ip.KeyPress(ESCAPE_KEY) || ip.QuitRequested() || SDL_QuitRequested())
    {
        quitRequested = true;
    }
    if (ip.KeyPress(SPACE_KEY))
    {
        popRequested = true;
        TitleState* stage = new TitleState();
        Game::GetInstance().Push(stage);
    }
    // UpdateArray(dt);
}

void EndState::LoadAssets(){}
void EndState::Render(){
    RenderArray();
}
void EndState::Pause(){}
void EndState::Resume(){}