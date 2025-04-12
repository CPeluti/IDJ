#pragma once
#include "SDL2/SDL_gpu.h"
#define INCLUDE_SDL_IMAGE
#include <string>
#include "SDL_include.h"
#include "Vec2.h"
#include "Shader.h"
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
        void SetFlip(SDL_RendererFlip flip);
    private:
        int frameCountW;
        int frameCountH;
        GPU_Image* texture;
        int width;
        int height;
        GPU_Rect clipRect;
        SDL_RendererFlip flip;
        Vec2 scale;
};