#pragma once
class Animation {
    public:
        Animation(int frameStart, int frameEnd, float frameTime);
        int frameStart;
        int frameEnd;
        float frameTime;
};