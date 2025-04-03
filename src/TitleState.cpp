#define INCLUDE_SDL_IMAGE
#include "Core/SDL_include.h"
#include "Core/TitleState.h"
#include "Core/SpriteRenderer.h"
#include "Core/StageState.h"
#include "Core/InputManager.h"
#include "Core/Camera.h"
#include "Core/Game.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <cmath>

TitleState::TitleState()
{
    GameObject *start = new GameObject();
    SpriteRenderer *sr = new SpriteRenderer(*start, "resources/img/Title.png", 1, 1);
    sr->SetCameraFollower(true);
    start->AddComponent(sr);
    this->AddObject(start);

    GameObject* text = new GameObject();
    Text* textComponent = new Text(*text, "resources/font/neodgm.ttf", 20, Text::SOLID, "Press SPACEBAR to play again or ESC to leave",{255,255,255}, 1);
    text->AddComponent(textComponent);
    this->AddObject(text);
    text->box.Move({Camera::pos.x+Game::GetInstance().GetWindowSize().x/2, Camera::pos.y+Game::GetInstance().GetWindowSize().y/2});
    text->z = 1;


    {
        started = true;
        m_Particle.SizeBegin = 4.0f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 1.0f;
        m_Particle.LifeTime = 1.0f;
        m_Particle.Velocity = { 0.0f, 0.0f };
        m_Particle.VelocityVariation = { 10.0f, 10.0f };
        m_Particle.Position = { 0.0f, 0.0f };
        m_Particle.VelocityFunction = {[](float x){return sin(8*x);},[](float x){return 1;}};
    }
    GameObject* particles = new GameObject();
    ParticleSystem* ps = new ParticleSystem(*particles, m_Particle);
    particles->AddComponent(ps);
    particles->box.Move({(Camera::pos.x+Game::GetInstance().GetWindowSize().x/2)-50, (Camera::pos.y+Game::GetInstance().GetWindowSize().y/2)-50});
    particles->z = 1;
    particlesSystem = this->AddObject(particles);
}
TitleState::~TitleState()
{
    this->objectArray.clear();
}
void TitleState::LoadAssets() {}
void TitleState::Update(float dt)
{
    InputManager &ip = InputManager::GetInstance();

    if (ip.KeyPress(ESCAPE_KEY) || ip.QuitRequested() || SDL_QuitRequested())
    {
        quitRequested = true;
    }
    if (ip.KeyPress(SPACE_KEY))
    {
        popRequested = true;
        StageState* stage = new StageState();
        Game::GetInstance().Push(stage);
    }
    if(auto ps = particlesSystem.lock()){
        //TODO: rever questão do sistema de particula estar contido em um gameObject
        ps->box.Move({ip.GetMouseX(), ip.GetMouseY()});
        ParticleSystem* particles = (ParticleSystem*)ps->GetComponent("ParticleSystem");
        particles->SetExplosiveness(true);
        particles->SetOneshot(true);
        
        particles->Play();

    }
    UpdateArray(dt);

}
void TitleState::Render()
{
    RenderArray();
}

void TitleState::Start()
{
    LoadAssets();
    StartArray();
    if(auto ps = particlesSystem.lock()){
        ParticleSystem* particles = (ParticleSystem*)ps->GetComponent("ParticleSystem");
        particles->SetAmount(30);
    }
}

void TitleState::Resume(){}
void TitleState::Pause(){}