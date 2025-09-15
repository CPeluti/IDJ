#define INCLUDE_SDL_MIXER
#include <memory>
#include <string>
#include "Core/SDL_include.h"
#include "Core/Music.h"
#include "Core/Resources.h"
Music::Music (){
    music = nullptr;
}
Music::Music (std::string file){
    music = nullptr;
    Open(file);
}
void Music::Stop (int ms){
    Mix_FadeOutMusic(ms);
}
Music::~Music (){
    Music::Stop(0);
     //Mix_FreeMusic(music);
}
void Music::Play (int times){
    if(IsOpen()){
        Mix_PlayMusic(music, times);
    }
}
void Music::Open (std::string file){
    // music = Mix_LoadMUS(file.c_str());
    music = Resources::GetMusic(file);
}
bool Music::IsOpen (){
    return music != nullptr;
}