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
    this->associated.subject.addObserver(this);
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
    lifebar.x -= Camera::pos.x;
    lifebar.y -= Camera::pos.y;
    SDL_Rect barToFill = {lifebar.x, lifebar.y, (int)(lifebar.w), (int)(lifebar.h)};
    barToFill.x = lifebar.x + (lifebar.w / 2) - (barToFill.w / 2);
    barToFill.y = lifebar.y + (lifebar.h / 2) - (barToFill.h / 2);
    barToFill.w = barToFill.w * current / total;

    SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 0, 0, 0, 255);
    SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &lifebar);
    SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, 255);
    SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &barToFill);
}

void Lifebar::setAmount(float amount)
{
    current = amount;
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
