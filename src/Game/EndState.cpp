#include "Core/Game.h"
#include "Core/Camera.h"
#include "Core/GameObject.h"
#include "Core/SpriteRenderer.h"
#include "Core/InputManager.h"

#include "Game/TitleState.h"
#include "Game/EndState.h"
#include "Game/GameData.h"

EndState::EndState(){
    if(GameData::playerWon){
        std::shared_ptr<GameObject> start = std::make_shared<GameObject>();
        std::shared_ptr<SpriteRenderer> sr =  std::make_shared<SpriteRenderer>(*start, "resources/img/Win.png", 1, 1);
        sr->SetCameraFollower(true);
        start->AddComponent(sr);
        this->AddObject(start);
        
        backgroundMusic = Music("resources/audio/win.ogg");

    }else {
        std::shared_ptr<GameObject> start = std::make_shared<GameObject>();
        std::shared_ptr<SpriteRenderer> sr =  std::make_shared<SpriteRenderer>(*start, "resources/img/Lose.png", 1, 1);
        sr->SetCameraFollower(true);
        start->AddComponent(sr);
        this->AddObject(start);

        backgroundMusic = Music("resources/audio/lose.ogg");
    }
    std::shared_ptr<GameObject> text = std::make_shared<GameObject>();
    std::shared_ptr<Text> textComponent = std::make_shared<Text>(*text, "resources/font/neodgm.ttf", 20, Text::SOLID, "Press SPACEBAR to play again or ESC to leave",SDL_Color{255,255,255}, 1);
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
        Game::GetInstance().Push(std::make_unique<TitleState>());
    }
    // UpdateArray(dt);
}

void EndState::LoadAssets(){}
void EndState::Render(){
    RenderArray();
}
void EndState::Pause(){}
void EndState::Resume(){}