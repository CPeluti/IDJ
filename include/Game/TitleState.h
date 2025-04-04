#pragma once
#include "Core/State.h"
#include "Core/Text.h"
#include "Core/ParticleSystem.h"

class TitleState : public State{
    public:
        TitleState();
        ~TitleState();

        void LoadAssets();
        void Update(float dt);
        void Render();

        void Start();
        void Resume();
        void Pause();
        std::weak_ptr<GameObject> particlesSystem;
        ParticleData m_Particle;
};