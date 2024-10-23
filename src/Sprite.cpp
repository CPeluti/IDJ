#define INCLUDE_SDL_IMAGE
#include <string>
#include "SDL_include.h"
#include <iostream>
#include "Game.h"
Sprite::Sprite()
{
    texture = nullptr;
}
Sprite::Sprite(std::string file, int frameCountW, int frameCountH)
{
    this->SetFrameCount(frameCountW, frameCountH);
    texture = nullptr;
    Open(file);
}
Sprite::~Sprite()
{
    if (IsOpen())
    {
        SDL_DestroyTexture(texture);
    }
}
void Sprite::Open(std::string file)
{
    if (IsOpen())
    {
        SDL_DestroyTexture(texture);
    }
    texture = IMG_LoadTexture(Game::GetInstance().GetRenderer(), file.c_str());
    if (!IsOpen())
    {
        std::cout << "Error loading texture: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    SetClip(0, 0, width, height);
}
void Sprite::SetClip(int x, int y, int w, int h)
{
    clipRect.x = x;
    clipRect.y = y;
    clipRect.w = w;
    clipRect.h = h;
}
void Sprite::SetFrame(int frame)
{

    int w = width / frameCountW;
    int h = height / frameCountH;
    int x = (frame % frameCountW) * w;
    int y = (frame / frameCountW) * h;
    SetClip(x, y, w, h);
}
void Sprite::SetFrameCount(int frameCountW, int frameCountH)
{
    this->frameCountH = frameCountH;
    this->frameCountW = frameCountW;
}
void Sprite::Render(int x, int y, int w, int h)
{
    SDL_Rect dstRect;
    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = clipRect.w;
    dstRect.h = clipRect.h;
    SDL_RenderCopy(Game::GetInstance().GetRenderer(), texture, &clipRect, &dstRect);
}
int Sprite::GetWidth()
{
    return width / frameCountW;
}
int Sprite::GetHeight()
{
    return height / frameCountH;
}
bool Sprite::IsOpen()
{
    return texture != nullptr;
}
