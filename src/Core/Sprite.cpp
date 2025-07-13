#include <string>
#include <iostream>

#include "SDL_gpu.h"

#define INCLUDE_SDL_IMAGE
#include "Core/SDL_include.h"

#include "Core/Game.h"
#include "Core/Resources.h"
#include "Core/Camera.h"
#include "Core/Log.h"
Sprite::Sprite()
{
    frameCountH = 1;
    frameCountW = 1;
    cameraFollower = false;
    texture = nullptr;
    scale = {1, 1};
    flip = SDL_FLIP_NONE;
}
Sprite::Sprite(std::string file, int frameCountW, int frameCountH)
{
    this->SetFrameCount(frameCountW, frameCountH);
    texture = nullptr;
    cameraFollower = false;
    Open(file);
    scale = {1, 1};
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
    this->width = texture->w;
    this->height = texture->h;
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
	//Vec2 screenSize = Game::GetInstance().GetWindowSize();
 //   Vec2 ul_corner = Camera::pos/Camera::zoom;
 //   Vec2 ur_corner = { Camera::pos.x / Camera::zoom + screenSize.x, Camera::pos.y / Camera::zoom };
 //   Vec2 dl_corner = { Camera::pos.x / Camera::zoom, Camera::pos.y / Camera::zoom + screenSize.y};
 //   Vec2 dr_corner = Camera::pos / Camera::zoom + screenSize;
 //   if (pos.x < ul_corner.x || pos.x > dr_corner.x || pos.y < ul_corner.y || pos.y > dr_corner.y)
 //   {
 //       return; // Outside camera view
	//}
    GPU_Rect dstRect = {(float)pos.x * Camera::zoom, (float)pos.y * Camera::zoom, (float)(clipRect.w * scale.x * Camera::zoom), (float)(clipRect.h * scale.y * Camera::zoom)};
    if (!cameraFollower)
    {
        dstRect.y -= Camera::pos.y;
        dstRect.x -= Camera::pos.x;
    }
    // std::cout << "x: " << x << std::endl << " y: " << y << std::endl << " w: " << w << std::endl << " h: " << h << std::endl;
    
    if(auto shader = m_shader.lock())
    {
        shader->Apply();
    }
    else {
        GPU_ActivateShaderProgram(0, NULL);
    }
    GPU_BlitRectX(texture, &clipRect, Game::GetInstance().GetGPUTarget(), &dstRect, angle, clipRect.x + (clipRect.w / 2), clipRect.y + (clipRect.h / 2), flip);
    if (auto shader = m_shader.lock())
    {
        shader->Remove();
    }
}
int Sprite::GetWidth()
{
    return (width * scale.x / frameCountW);
}
int Sprite::GetHeight()
{
    return (height * scale.y / frameCountH);
}
bool Sprite::IsOpen()
{
    return texture != nullptr;
}

void Sprite::SetScale(float scaleX, float scaleY)
{
    scale = {scaleX ? scaleX : scale.x, scaleY ? scaleY : scale.y};
}

void Sprite::SetFlip(SDL_RendererFlip flip)
{
    this->flip = flip;
}

void Sprite::SetShader(std::shared_ptr<Shader> shader)
{
    m_shader = shader;
}