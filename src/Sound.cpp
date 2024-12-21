#define INCLUDE_SDL_MIXER
#include "SDL_include.h"
#include "Sound.h"
#include <iostream>
#include "Resources.h"
Sound::Sound(){
    chunk = nullptr;
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
    if(chunk != nullptr)
    Mix_HaltChannel(channel);
}

void Sound::Open(std::string file){
    chunk = Resources::GetSound(file);
}