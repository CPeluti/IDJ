#include "Core/Component.h"
#include "Core/Observer.h"
#include "Core/GameObject.h"
#include "Core/Game.h"
#include "Core/Camera.h"
#include <iostream>

#include "Game/Lifebar.h"
#include "Game/Character.h"

Lifebar::Lifebar(GameObject &associated, float max, Vec2 size, Vec2 offset) : Component(associated), total(max), size(size), offset(offset)
{
    current = max;
    //LOG_INFO(size);
}

bool Lifebar::Is(std::string type)
{
    return type == "Lifebar";
}

void Lifebar::Update(float dt)
{
    lifebar.x = (int)(this->associated.box.GetPos().x - offset.x);
    lifebar.y = (int)(this->associated.box.GetPos().y - offset.y);
}

void Lifebar::Render()
{
    std::cout << m_enableLifebar << std::endl;
    if (!m_enableLifebar)
        return;

    SDL_Rect barToFill = {lifebar.x, lifebar.y, (int)(lifebar.w), (int)(lifebar.h)};
    SDL_Rect bg = {lifebar.x, lifebar.y, (int)(lifebar.w), (int)(lifebar.h)};
    barToFill.x = lifebar.x + (lifebar.w / 2) - (barToFill.w / 2);
    barToFill.y = lifebar.y + (lifebar.h / 2) - (barToFill.h / 2);
    barToFill.w = barToFill.w * current / total;

    barToFill.x *= Camera::zoom;
    barToFill.y *= Camera::zoom;
    barToFill.w *= Camera::zoom;
    barToFill.h *= Camera::zoom;

    bg.x *= Camera::zoom;
    bg.y *= Camera::zoom;
    bg.w *= Camera::zoom;
    bg.h *= Camera::zoom;

    barToFill.x -= Camera::pos.x;
    barToFill.y -= Camera::pos.y;

    bg.x -= Camera::pos.x;
    bg.y -= Camera::pos.y;
    // SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 0, 0, 0, 255);
    GPU_RectangleFilled(Game::GetInstance().GetGPUTarget(), bg.x, bg.y, bg.x + bg.w, bg.y + bg.h, {0, 0, 0, 255});
    // SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, 255);
    GPU_RectangleFilled(Game::GetInstance().GetGPUTarget(), barToFill.x, barToFill.y, barToFill.x + barToFill.w, barToFill.y + barToFill.h, {255, 0, 0, 255});
}

void Lifebar::setAmount(float amount)
{
    current = amount > 0 ? amount : 0;
}

void Lifebar::setMax(float max)
{
    total = max;
}

void Lifebar::Start()
{
    lifebar = {
        (int)(this->associated.box.GetPos().x - offset.x),
        (int)(this->associated.box.GetPos().y - offset.y),
        (int)size.x,
        (int)size.y};
}

// void Lifebar::onNotify(const Component& component, Observer::Event event){
//     switch(event){
//         case Observer::Event::onTakeDamage:
//             this->setAmount(((Character&) component).getHP());
//             break;
//         default:
//             break;
//     }
// }