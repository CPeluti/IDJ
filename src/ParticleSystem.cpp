#include "Core/ParticleSystem.h"
#include "Core/Random.h"

ParticleSystem::ParticleSystem(GameObject& associated): Component(associated){
    m_ParticlePool.resize(1000);
    m_Sprite = new Sprite("resources/img/shimmer.bmp");
}


void ParticleSystem::Update(float dt){
    for(auto& particle : m_ParticlePool){
        if(!particle.Active) continue;

        if(particle.LifeTimeRemaining <= 0.0f){
            particle.Active = false;
            continue;
        }

        particle.LifeTimeRemaining -= dt;
        particle.Position = particle.Position + (particle.Velocity*50 * dt);
        particle.Rotation += 0.01f * dt;
    }
}

void ParticleSystem::Render(){
    for(auto& particle : m_ParticlePool){
        if(!particle.Active) continue;
        float current = particle.LifeTimeRemaining/particle.LifeTime;
        float size = Vec2::lerp(particle.SizeBegin, particle.SizeEnd, current);
        m_Sprite->SetScale(size,size);
        m_Sprite->Render(particle.Position, associated.box.GetSize() + Vec2(size,size), particle.Rotation);
    }
}

void ParticleSystem::Start(){}


void ParticleSystem::Emit(const ParticleData& particleData){
    Particle& particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;

    float r = Random::Float();

    particle.Position = associated.box.center();
    particle.Rotation = Random::Float() * 2.0f * M_PI;

    particle.Velocity = particleData.Velocity;
    particle.Velocity.x = particleData.VelocityVariation.x * (Random::Float()-0.5f);
    particle.Velocity.y = particleData.VelocityVariation.y * (Random::Float()-0.5f);

    particle.LifeTime = particleData.LifeTime;
    particle.LifeTimeRemaining = particleData.LifeTime;
    particle.SizeBegin = particleData.SizeBegin + particleData.SizeVariation * (Random::Float()-0.5f);
    particle.SizeEnd = particleData.SizeEnd;

    m_PoolIndex = --m_PoolIndex%m_ParticlePool.size();

}
