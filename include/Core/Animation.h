#pragma once

#define INCLUDE_SDL
#include "SDL_include.h"
class Animation {
    public:
        Animation(int frameStart, int frameEnd, float frameTime, SDL_FlipMode flip = SDL_FLIP_NONE);
        int frameStart;
        int frameEnd;
        float frameTime;
        SDL_FlipMode flip;
};