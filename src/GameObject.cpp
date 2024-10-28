#include "GameObject.h"
#include <iostream>
// #include "Component.h"
GameObject::GameObject()
{
    this->isDead = false;
}

GameObject::~GameObject()
{
    for (int i = (int)this->components.size(); i >= 0; i--)
    {
        delete this->components[i];
    }
    this->components.clear();
}

void GameObject::Update(float dt)
{
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        this->components[i]->Update(dt);
    }
}

void GameObject::Render()
{
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        std::cout << "rendering " << this->components[i]->Is("TileMap");
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

void GameObject::AddComponent(Component *cpt)
{
    this->components.push_back(cpt);
}

void GameObject::RemoveComponent(Component *cpt)
{
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        if (this->components[i] == cpt)
        {
            this->components.erase(this->components.begin() + i);
        }
    }
}

Component *GameObject::GetComponent(std::string type)
{
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        if (this->components[i]->Is(type))
        {
            return this->components[i];
        }
    }
    return nullptr;
}