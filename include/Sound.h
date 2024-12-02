#pragma once
#define INCLUDE_SDL_MIXER
#include "SDL_Include.h"
#include <string>
class Sound {
    public:
        Sound();
        Sound(std::string file);
        ~Sound();
        void Play(int times = 1);
        void Stop();
        void Open();
        bool IsOpen();
    
    private:
        Mix_Chunk* chunk;
        int channel;

};