#pragma once
#include <string>
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
    private:
        int frameCountW;
        int frameCountH;
        SDL_Texture* texture;
        int width;
        int height;
        SDL_Rect clipRect;
};