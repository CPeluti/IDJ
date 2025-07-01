#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Vec2.h"
#include "Rect.h"

#include <vector>
#include "spdlog/fmt/fmt.h"

enum class ColliderType {
	none = 0,
	physics
};

enum class ColliderFunctionType
{
    OnCollision,
    OnInteraction,
};

class Collider : public Component, public std::enable_shared_from_this<Component>
{
public:
    Collider(GameObject &associated, std::vector<std::string> collisionLayers, std::string tag, Event *event, Vec2 size = {0, 0}, Vec2 scale = {1, 1}, Vec2 offset = {0, 0}, float weight = 0.5f);
    ~Collider();
    Rect box;
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    void SetScale(Vec2 scale);
    void SetOffset(Vec2 scale);
    inline Vec2 GetSize(){return size;}
    inline Vec2 GetOffset(){return offset;}
    inline void SetSize(Vec2 newSize){size = newSize;}
    Event *GetEvent() { return m_event; };
    std::string GetTag();
    // ColliderFunctionType GetColliderFunctionType(ColliderFunctionType functionType) { return m_functionType; };

    inline std::string ToString() const{
        return fmt::format("\nCollider\n\tTag: {}\n\tPos: ({},{})\n\tSize: ({},{})\n\tOffset: ({},{})",m_tag, this->associated.box.GetPos().x, this->associated.box.GetPos().y, size.x, size.y, offset.x, offset.y);
    }
    float weight;
private:
    Vec2 scale;
    Vec2 offset;
    Vec2 size;
    std::vector<std::string> m_collisionLayers;
    std::string m_tag;
    // ColliderFunctionType m_functionType;
    Event *m_event;
    
};


inline std::string format_as(const Collider &c)
{
    return c.ToString();
}