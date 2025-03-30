#define INCLUDE_SDL_IMAGE
#include <string>
#include "Core/SDL_include.h"
#include <iostream>
#include "Core/Game.h"
#include "Core/Resources.h"
#include "Core/Camera.h"
Sprite::Sprite()
{
    frameCountH = 1;
    frameCountW = 1;
    cameraFollower = false;
    texture = nullptr;
    scale = {1,1};
    flip = SDL_FLIP_NONE;
}
Sprite::Sprite(std::string file, int frameCountW, int frameCountH)
{
    this->SetFrameCount(frameCountW, frameCountH);
    texture = nullptr;
    cameraFollower = false;
    Open(file);
    scale = {1,1};
    flip = SDL_FLIP_NONE;
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
    SetClip({0, 0}, {width, height});
}
void Sprite::SetClip(Vec2 pos, Vec2 size)
{
    clipRect.x = pos.x;
    clipRect.y = pos.y;
    clipRect.w = size.x;
    clipRect.h = size.y;
}
void Sprite::SetFrame(int frame)
{

    int w = width / frameCountW;
    int h = height / frameCountH;
    int x = (frame % frameCountW) * w;
    int y = (frame / frameCountW) * h;
    SetClip({x, y}, {w, h});
}
void Sprite::SetFrameCount(int frameCountW, int frameCountH)
{
    this->frameCountH = frameCountH;
    this->frameCountW = frameCountW;
}
void Sprite::Render(Vec2 pos, Vec2 size, float angle)
{
    SDL_Rect dstRect = {(int)pos.x,(int)pos.y, (int)(clipRect.w * scale.x), (int)(clipRect.h * scale.y)};
    if(!cameraFollower){
        dstRect.y -= Camera::pos.y;
        dstRect.x -= Camera::pos.x;   
    }
    // std::cout << "x: " << x << std::endl << " y: " << y << std::endl << " w: " << w << std::endl << " h: " << h << std::endl;
    SDL_RenderCopyEx(Game::GetInstance().GetRenderer(), texture, &clipRect, &dstRect, angle, nullptr, flip);
}
int Sprite::GetWidth()
{
    return (width*scale.x / frameCountW);
}
int Sprite::GetHeight()
{
    return (height*scale.y / frameCountH);
}
bool Sprite::IsOpen()
{
    return texture != nullptr;
}

void Sprite::SetScale(float scaleX, float scaleY){
    scale = {scaleX?scaleX:scale.x,scaleY?scaleY:scale.y};
}

void Sprite::SetFlip(SDL_RendererFlip flip){
    this->flip = flip;
}