#pragma once
#define INCLUDE_SDL_IMAGE
#include <string>
#include "SDL_include.h"
#include "Vec2.h"
class Sprite{
    public:
        Sprite();
        Sprite(std::string file, int frameCountW = 1, int frameCountH = 1);
        ~Sprite();
        void Open(std::string file);
        void SetClip(Vec2 pos, Vec2 size);
        void Render(Vec2 pos, Vec2 size, float angle = 0);
        void SetFrame (int frame);
        void SetFrameCount (int frameCountW, int frameCountH);
        int GetWidth();
        int GetHeight();
        bool IsOpen();
        bool cameraFollower;
        void SetScale(float scaleX, float scaleY);
        Vec2 GetScale();
        void SetFlip(SDL_FlipMode flip);
    private:
        int frameCountW;
        int frameCountH;
        SDL_Texture* texture;
        float width;
        float height;
        SDL_FRect clipRect;
        SDL_FlipMode flip;
        Vec2 scale;
};