#define INCLUDE_SDL_IMAGE
#include <string>
#include "SDL_include.h"
#include <iostream>
#include "Game.h"
#include "Resources.h"
#include "Camera.h"
Sprite::Sprite()
{
    frameCountH = 1;
    frameCountW = 1;
    cameraFollower = false;
    texture = nullptr;
}
Sprite::Sprite(std::string file, int frameCountW, int frameCountH)
{
    this->SetFrameCount(frameCountW, frameCountH);
    texture = nullptr;
    cameraFollower = false;
    Open(file);
}
Sprite::~Sprite()
{
    // if (IsOpen())
    // {
    //     SDL_DestroyTexture(texture);
    // }
}
void Sprite::Open(std::string file)
{
    texture = Resources::GetImage(file);
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
    SDL_Rect dstRect = {x,y, clipRect.w, clipRect.h};
    if(!cameraFollower){
        dstRect.y -= Camera::pos.y;
        dstRect.x -= Camera::pos.x;   
    }
    // std::cout << "x: " << x << std::endl << " y: " << y << std::endl << " w: " << w << std::endl << " h: " << h << std::endl;
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
