#pragma once
#include <string>
#include <vector>
#include "Rect.h"
#include "Component.h"
#include "Subject.h"
// class Component;

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
    inline void SetSpeed(Vec2 newSpeed){ m_speed = newSpeed; }
    inline Vec2 GetSpeed(){ return m_speed;}
    Component *GetComponent(std::string type);
    void Start();
    Rect box;
    // void NotifyCollision(GameObject& other);
    int z;
    double angleDeg;
    Subject subject;
    
private:
    Vec2 m_speed;
    bool started;
    std::vector<Component *> components;
    bool isDead;
};