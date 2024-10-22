#define INCLUDE_SDL_MIXER
#include <memory>
#include <string>
#include "SDL_include.h"
#include "Music.h"
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
    Music::Stop();
    Mix_FreeMusic(music);
}
void Music::Play (int times){
    if(IsOpen()){
        Mix_PlayMusic(music, times);
    }
}
void Music::Open (std::string file){
    music = Mix_LoadMUS(file.c_str());
}
bool Music::IsOpen (){
    return music != nullptr;
}