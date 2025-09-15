#pragma once
#include <memory>
#include "GameObject.h"
#include "Animation.h"
#include "Sprite.h"
#include "Component.h"

class SpriteRenderer : public Component
{
public:
    SpriteRenderer(GameObject &associated);
    SpriteRenderer(GameObject &associated, std::string file, int frameCountW = 1, int frameCountH = 1 , float baseAngle = 0);
    void Open(std::string file);
    void SetFrameCount(int frameCountW, int frameCountH);
    void Update(float dt);
    void Render();

    inline std::weak_ptr<Shader> GetShader()
    {
        return m_shader;
    }

    bool Is(std::string type);
    void SetCameraFollower(bool state);
    void SetScale(float scaleX, float scaleY);
    void SetFrame(int frame, SDL_RendererFlip flip);
    bool enabled = true;

private:
    float baseAngle = 0;
    Sprite *sprite;
    std::shared_ptr<Shader> m_shader;
};