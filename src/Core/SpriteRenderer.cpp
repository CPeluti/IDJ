#include "Core/SpriteRenderer.h"
#include "Core/Component.h"
#include "Core/Log.h"
#include <iostream>
SpriteRenderer::SpriteRenderer(GameObject &associated) : Component(associated)
{
    this->sprite = new Sprite();
}

SpriteRenderer::SpriteRenderer(GameObject& associated, std::string file, int frameCountW, int frameCountH, float baseAngle) : Component(associated), baseAngle(baseAngle)
{
    // this->sprite.Open(file);
    // this->sprite.SetFrameCount(frameCountW, frameCountH);
    this->sprite = new Sprite(file, frameCountW, frameCountH);

    associated.box.SetSize({sprite->GetWidth(), sprite->GetHeight()});
    m_shader = std::make_shared<Shader>();
}

void SpriteRenderer::SetFrameCount(int frameCountW, int frameCountH)
{
    this->sprite->SetFrameCount(frameCountW, frameCountH);
}

void SpriteRenderer::Open(std::string file)
{
    this->sprite->Open(file);
}

bool SpriteRenderer::Is(std::string type)
{
    return type == "SpriteRenderer";
}

void SpriteRenderer::Update(float dt) {}

void SpriteRenderer::Render()
{
    if (!enabled)
        return;
    sprite->SetShader(m_shader);
    sprite->Render(associated.box.GetPos() - Vec2{.0f, this->associated.height }, associated.box.GetSize(), associated.angleDeg + baseAngle);
}

void SpriteRenderer::SetFrame(int frame, SDL_RendererFlip flip)
{
    sprite->SetFrame(frame);
    sprite->SetFlip(flip);
}

void SpriteRenderer::SetCameraFollower(bool state)
{
    sprite->cameraFollower = state;
}

void SpriteRenderer::SetScale(float scaleX, float scaleY)
{
    Vec2 center = associated.box.center();

    sprite->SetScale(scaleX, scaleY);

    associated.box.SetSize({sprite->GetWidth(), sprite->GetHeight()});
    Vec2 size = associated.box.GetSize() / 2;

    associated.box.RawMove(center + size);
}

// newBoxPos-NewBoxSize/2 = oldBoxPos - oldBoxSize/2