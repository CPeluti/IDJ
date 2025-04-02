#include "Vec2.h"
#include "Component.h"
#include "Sprite.h"
#include "GameObject.h"
#include "Timer.h"
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

    Modifier VelocityFunction = {[](float x){return 1.0f;}, [](float x){return 1.0f;}};

};

class ParticleSystem : public Component {
    public:
        ParticleSystem(GameObject& associated, ParticleData pd);

        void Update(float dt);
        void Render();
        void Start();
        inline void Play() {m_Emit = true;};
        inline void Stop() {m_Emit = false;};
        inline void SetAmount(int amount) {m_Amount = amount;}
        inline void SetExplosiveness(bool explosiveness) {m_Explosiveness = explosiveness;}
        inline void SetOneshot(bool oneshot) {m_Oneshot = oneshot;}
        inline bool Is(std::string type) {return type == "ParticleSystem";}

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
        int m_Amount = 1;
        int m_EmittedAmount = 0;
        bool m_Explosiveness = false;
        bool m_Oneshot = false;
        bool m_Emit = false;
        ParticleData m_Particle;
        Timer m_EmitTimer;
};