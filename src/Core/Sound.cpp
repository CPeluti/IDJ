#define INCLUDE_SDL_MIXER
#include "Core/SDL_include.h"
#include <iostream>
#include "Core/Sound.h"
#include "Core/Resources.h"
Sound::Sound(){
    chunk = nullptr;
    channel = -1;
}

Sound::Sound(std::string file): Sound(){
    Open(file);
}

Sound::~Sound() {
    if(chunk != nullptr){
        Stop();
        // Mix_FreeChunk(chunk);
    }
}

void Sound::Play(int times){
    channel = Mix_PlayChannel(-1, chunk, times-1);
}

void Sound::Stop(){
    if(chunk != nullptr && channel >= 0)
    Mix_HaltChannel(channel);
}

void Sound::Open(std::string file){
    chunk = Resources::GetSound(file);
}