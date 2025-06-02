#pragma once
#include "Component.h"
#include "Vec2.h"
#include "Rect.h"

#include <vector>

enum class ColliderFunctionType
{
    OnCollision,
    OnInteraction,
};

class Collider : public Component
{
public:
    Collider(GameObject &associated, std::vector<std::string> collisionLayers, Event *event, Vec2 size = {0, 0}, Vec2 scale = {1, 1}, Vec2 offset = {0, 0});
    ~Collider();
    Rect box;
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    void SetScale(Vec2 scale);
    void SetOffset(Vec2 scale);
    Event *GetEvent() { return m_event; };
    // ColliderFunctionType GetColliderFunctionType(ColliderFunctionType functionType) { return m_functionType; };

private:
    Vec2 scale;
    Vec2 offset;
    Vec2 size;
    std::vector<std::string> m_collisionLayers;
    // ColliderFunctionType m_functionType;
    Event *m_event;
};