#pragma once
#include <string>
// #include "GameObject.h"
class GameObject;
class Component
{
public:
    Component(GameObject &associated);
    virtual ~Component();
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Start();
    virtual bool Is(std::string type) = 0;
    virtual void NotifyCollision (GameObject& other);

protected:
    GameObject &associated;
};