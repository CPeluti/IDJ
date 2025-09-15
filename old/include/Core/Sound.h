#pragma once
#define INCLUDE_SDL_MIXER
#include "SDL_include.h"
#include <string>
class Sound {
    public:
        static int usedChannels;
        Sound();
        Sound(std::string file);
        ~Sound();
        void Play(int times = 1);
        void Stop();
        void Open(std::string file);
        bool IsOpen();
		bool IsPlaying() const;
        inline int getChannel() const { return channel; }
    private:
        Mix_Chunk* chunk;
        int channel;

};