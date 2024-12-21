#pragma once
#define INCLUDE_SDL_IMAGE
#include <string>
#include "SDL_include.h"
class Sprite{
    public:
        Sprite();
        Sprite(std::string file, int frameCountW = 1, int frameCountH = 1);
        ~Sprite();
        void Open(std::string file);
        void SetClip(int x, int y, int w, int h);
        void Render(int x, int y, int w, int h);
        void SetFrame (int frame);
        void SetFrameCount (int frameCountW, int frameCountH);
        int GetWidth();
        int GetHeight();
        bool IsOpen();
        bool cameraFollower;
    private:
        int frameCountW;
        int frameCountH;
        SDL_Texture* texture;
        int width;
        int height;
        SDL_Rect clipRect;
};