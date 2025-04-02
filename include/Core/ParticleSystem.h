#include "Vec2.h"
#include "Component.h"
#include "Sprite.h"
#include "GameObject.h"
#include <functional>

struct Modifier{
    std::function<float(float)> functionX;
    std::function<float(float)> functionY;
};

struct ParticleData {

    Vec2 Position;
    Vec2 Velocity, VelocityVariation;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;

    Modifier VelocityFunction;

};

class ParticleSystem : public Component {
    public:
        ParticleSystem(GameObject& associated);

        void Update(float dt);
        void Render();
        void Start();
        bool Is(std::string type) {return type == "ParticleSystem";}

        void Emit(const ParticleData& particleData);

    
    private:
        struct Particle {
            Vec2 Position;
            Vec2 Velocity;
            float Rotation = 0.0f;
            float SizeBegin, SizeEnd;

            float LifeTime = 1.0f;
            float LifeTimeRemaining = 0.0f;

            bool Active = false;
            Modifier velocityFunction;
        };

        std::vector<Particle> m_ParticlePool;
        int m_PoolIndex = 999;
        Sprite* m_Sprite;

};