#include "Core/Animation.h"

Animation::Animation(int frameStart, int frameEnd, float frameTime, bool repeat, SDL_RendererFlip flip) {
    this->frameStart = frameStart;
    this->frameEnd = frameEnd;
    this->frameTime = frameTime;
    this->flip = flip;
    this->repeat = repeat;
}

