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
    // try{
    //     chunk = Mix_LoadWAV(file.c_str());
    // } catch (std::string e){
    //     std::cout << "Failed to open sound: " << SDL_GetError() << std::endl;
    // }
    chunk = Resources::GetSound(file);
    
}