#include <string>
#include "GameObject.h"
class Bullet : public Component {
    public:
        Bullet(GameObject& associated, float angle, float speed, int damage, float maxDistance);
        void Update(float dt);
        void Render();
        bool Is(std::string type);
        int GetDamage();
        void NotifyCollision(GameObject& other);
    private:
        Vec2 speed;
        float distanceLeft;
        int damage;
};