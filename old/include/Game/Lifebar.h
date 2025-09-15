#pragma once
#define INCLUDE_SDL_IMAGE
#include "Core/SDL_include.h"
#include "Core/Component.h"
#include "Core/Vec2.h"
#include "Core/Observer.h"
class Lifebar : public Component
{
public:
    Lifebar(GameObject &associated, float total, Vec2 size, Vec2 offset, SDL_Color color = {255,0,0,255}, bool boss = false);
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
	SDL_Color m_color = { 255, 0, 0, 255 }; // Default color red
	bool m_boss = false; // If true, this is a boss lifebar
};