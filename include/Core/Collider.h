#pragma once
#include "Component.h"
#include "Vec2.h"
#include "Rect.h"

#include <vector>
class Collider: public Component {
    public:
        Collider(GameObject& associated, std::vector<std::string> collisionLayers, Vec2 scale = {1,1}, Vec2 offset = {0,0});
        ~Collider();
        Rect box;
        void Update(float dt);
        void Render();
        bool Is(std::string type);
        void SetScale(Vec2 scale);
        void SetOffset(Vec2 scale);
    private:
        Vec2 scale;
        Vec2 offset;
        std::vector<std::string> m_collisionLayers;
};