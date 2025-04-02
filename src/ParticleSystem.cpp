#include "Core/ParticleSystem.h"
#include "Core/Random.h"
#include "Core/Log.h"

ParticleSystem::ParticleSystem(GameObject& associated, ParticleData pd): Component(associated), m_EmitTimer(10){
    m_Particle = pd;
    m_ParticlePool.resize(1000);
    m_Sprite = new Sprite("resources/img/shimmer.bmp");
}


void ParticleSystem::Update(float dt){
    m_EmitTimer.SetAmount(1.0f/m_Amount);
    if(m_Emit){
        if((m_Amount - m_EmittedAmount > 0) && m_EmitTimer.Expired()){
            Emit(m_Particle);
            m_EmitTimer.Restart();
        }
    }
    for(auto& particle : m_ParticlePool){
        if(!particle.Active) continue;

        if(particle.LifeTimeRemaining <= 0.0f){
            particle.Active = false;
            continue;
        }

        particle.LifeTimeRemaining -= dt;
        float x = 1, y = 1; 
        if(particle.velocityFunction.functionX && particle.velocityFunction.functionY){
            x = particle.velocityFunction.functionX(particle.LifeTimeRemaining);
            y = particle.velocityFunction.functionY(particle.LifeTimeRemaining);
        }
        Vec2 newVelocity = {particle.Velocity.x * x, particle.Velocity.y * y };
        particle.Position = particle.Position + ( newVelocity*50 * dt);
        particle.Rotation += 0.01f * dt;
    }
    m_EmitTimer.Update(dt);
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

void ParticleSystem::Start(){
    m_EmitTimer.SetAmount(1/m_Amount);
    m_EmitTimer.SetTime(1/m_Amount);
}

void ParticleSystem::Emit(const ParticleData& particleData){
    Particle& particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;

    particle.velocityFunction = particleData.VelocityFunction;

    float r = Random::Float();

    particle.Position = associated.box.center();
    particle.Rotation = Random::Float() * 2.0f * M_PI;

    particle.Velocity = particleData.Velocity;
    particle.Velocity.x = particleData.Velocity.x+particleData.VelocityVariation.x * (Random::Float()-0.5f);
    particle.Velocity.y = particleData.Velocity.y+particleData.VelocityVariation.y * (Random::Float()-0.5f);

    particle.LifeTime = particleData.LifeTime;
    particle.LifeTimeRemaining = particleData.LifeTime;
    particle.SizeBegin = particleData.SizeBegin + particleData.SizeVariation * (Random::Float()-0.5f);
    particle.SizeEnd = particleData.SizeEnd;

    m_PoolIndex = --m_PoolIndex%m_ParticlePool.size();


    // reset the emitted particles count if oneshot is false
    m_EmittedAmount++;
    if(m_EmittedAmount >= m_Amount && !m_Oneshot) m_EmittedAmount = 0;
}
