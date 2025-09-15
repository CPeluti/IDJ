#define INCLUDE_SDL_MIXER
#include "Core/SDL_include.h"
#include <iostream>
#include "Core/Sound.h"
#include "Core/Resources.h"
#include "Core/Log.h"
int Sound::usedChannels = 0;
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
    channel = Mix_PlayChannel(-1, chunk, times - 1);
    usedChannels++;
}
bool Sound::IsPlaying() const{
    if(Mix_Playing(channel) == 0){
        return false;
	}
    return true;
}
void Sound::Stop(){
    if(chunk != nullptr && channel >= 0)
    Mix_HaltChannel(channel);
    usedChannels--;
}

void Sound::Open(std::string file){
    chunk = Resources::GetSound(file);
}