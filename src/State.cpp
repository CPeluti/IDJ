#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"
#include "State.h"
State::State (){
    quitRequested = false;
    bg = new Sprite();
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
    bg->Render(0, 0);
}
