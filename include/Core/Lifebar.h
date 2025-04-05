#pragma once
#define INCLUDE_SDL_IMAGE
#include "SDL_include.h"
#include "Component.h"
#include "Vec2.h"
#include "Observer.h"
class Lifebar : public Component
{
public:
    Lifebar(GameObject &associated, float total, Vec2 size, Vec2 offset);
    void Start();
    bool Is(std::string type);
    void Update(float dt);
    void Render();
    void setAmount(float amount);
    void setMax(float max);
    inline void SetEnableLifebar(bool enable) { m_enableLifebar = enable; }

private:
    SDL_Rect lifebar;
    Vec2 size;
    float total;
    float current;
    Vec2 offset;
    bool m_enableLifebar = true;
};