#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Sprite.h"
#include "Component.h"

class SpriteRenderer : public Component
{
public:
    SpriteRenderer(GameObject &associated);
    SpriteRenderer(GameObject &associated, std::string file, int frameCountW = 1, int frameCountH = 1);
    void Open(std::string file);
    void SetFrameCount(int frameCountW, int frameCountH);
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    void SetCameraFollower(bool state);
    void SetScale(float scaleX, float scaleY);
    void SetFrame(int frame, SDL_RendererFlip flip);
private:
    Sprite sprite;
};