#include "SpriteRenderer.h"
#include "Component.h"
#include <iostream>
SpriteRenderer::SpriteRenderer(GameObject &associated) : Component(associated)
{
    this->sprite = Sprite();
}

SpriteRenderer::SpriteRenderer(GameObject &associated, std::string file, int frameCountW, int frameCountH) : Component(associated), sprite(file, frameCountW, frameCountH)
{
    // this->sprite.Open(file);
    // this->sprite.SetFrameCount(frameCountW, frameCountH);

    associated.box.w = sprite.GetWidth();
    associated.box.h = sprite.GetHeight();
}

void SpriteRenderer::SetFrameCount(int frameCountW, int frameCountH)
{
    this->sprite.SetFrameCount(frameCountW, frameCountH);
}

void SpriteRenderer::Open(std::string file)
{
    this->sprite.Open(file);
}

bool SpriteRenderer::Is(std::string type)
{
    return type == "SpriteRenderer";
}

void SpriteRenderer::Update(float dt){}

void SpriteRenderer::Render()
{
    sprite.Render(associated.box.x, associated.box.y, associated.box.w, associated.box.h, associated.angleDeg);
}

void SpriteRenderer::SetFrame(int frame, SDL_RendererFlip flip)
{
    sprite.SetFrame(frame);
    sprite.SetFlip(flip);
}

void SpriteRenderer::SetCameraFollower(bool state){
    sprite.cameraFollower = state;
}

void SpriteRenderer::SetScale(float scaleX, float scaleY){
    Vec2 center = {
        abs(associated.box.x-associated.box.w/2),
        abs(associated.box.y-associated.box.h/2)
    };

    sprite.SetScale(scaleX,scaleY);

    associated.box.w = sprite.GetWidth();
    associated.box.h = sprite.GetHeight();

    associated.box.x = abs(center.x+associated.box.w/2);
    associated.box.y = abs(center.y+associated.box.h/2);
    std::cout << associated.box.y << " " << center.y << std::endl;
}

//newBoxPos-NewBoxSize/2 = oldBoxPos - oldBoxSize/2 