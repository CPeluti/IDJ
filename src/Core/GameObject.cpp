#include "Core/GameObject.h"
#include <iostream>
// #include "Component.h"
GameObject::GameObject()
{
    isDead = false;
    started = false;
    angleDeg = 0;
    z=1;
}

GameObject::~GameObject()
{
    for(auto component = components.begin(); component != components.end();)
    {
        component = components.erase(component);
    }
}

void GameObject::Start(){
    for(auto iter : components){
        iter->Start();
    }
    started = true;
}

void GameObject::Update(float dt)
{
    this->box.MoveToPos(m_speed);
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        this->components[i]->Update(dt);
    }
}

void GameObject::Render()
{
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        // std::cout << "rendering " << this->components[i]->Is("TileMap");
        this->components[i]->Render();
    }
}

bool GameObject::IsDead()
{
    return this->isDead;
}

void GameObject::RequestDelete()
{
    this->isDead = true;
}

void GameObject::AddComponent(std::shared_ptr<Component> cpt)
{
    this->components.emplace_back(cpt);
    if(started){
        cpt->Start();
    }
}

void GameObject::RemoveComponent(std::weak_ptr<Component> cpt)
{
    for (int i = 0; i < (int)this->components.size();)
    {
        if (this->components[i] == cpt.lock())
        {
            this->components.erase(this->components.begin() + i);
        } else {
            i++;
        }
    }
}

std::weak_ptr<Component> GameObject::GetComponent(std::string type)
{
    if(type == "Animator"){
        std::cout << "teste";
    }
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        if (this->components[i]->Is(type))
        {
            return this->components[i];
        }
    }
    return std::weak_ptr<Component>();
}

// void GameObject::NotifyCollision(GameObject& other)
// {
//     for (int i = 0; i < (int)this->components.size(); i++)
//     {
//         this->components[i]->NotifyCollision(other);
//     }
// }