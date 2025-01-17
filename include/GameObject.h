#pragma once
#include <string>
#include <vector>
#include "Rect.h"
// class Component;
#include "Component.h"

class GameObject
{
public:
    GameObject();
    ~GameObject();
    void Update(float dt);
    void Render();
    bool IsDead();
    void RequestDelete();
    void AddComponent(Component *cpt);
    void RemoveComponent(Component *cpt);
    Component *GetComponent(std::string type);
    void Start();
    Rect box;
    int z;
    double angleDeg;
private:
    bool started;
    std::vector<Component *> components;
    bool isDead;
};