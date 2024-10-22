#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"
#include "State.h"
#include <iostream>

State::State (){
    quitRequested = false;
    bg = new Sprite();
    music = new Music("resources/audio/BGM.wav");
    music->Play();
} 
bool State::QuitRequested (){
    return quitRequested;
}
void State::LoadAssets (){}
void State::Update (float dt){
    if (SDL_QuitRequested()){
        quitRequested = true;
    }
}
void State::Render (){
    bg->Open("resources/img/Background.png");
    bg->Render(0, 0);
}
