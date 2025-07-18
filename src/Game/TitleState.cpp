#include <iostream>
#include <set>
#include <algorithm>
#include <cmath>

#define INCLUDE_SDL_IMAGE
#include "Core/SDL_include.h"
#include "Core/SpriteRenderer.h"
#include "Core/InputManager.h"
#include "Core/Camera.h"
#include "Core/Game.h"

#include "Game/TitleState.h"
#include "Game/StageState.h"
TitleState::TitleState(): m_backgroundMusic("resources/audio/MainTheme.mp3")
{
    Camera::smoothness = 8.f;
    menu = std::make_shared<Menu>();

    std::shared_ptr<GameObject> start = std::make_shared<GameObject>();
    std::shared_ptr<SpriteRenderer> sr =  std::make_shared<SpriteRenderer>(*start, "resources/interface/tela_inicial.png", 1, 1);
	float factor_x = Game::GetInstance().GetWindowSize().x / start->box.GetSize().x;
	float factor_y = Game::GetInstance().GetWindowSize().y / start->box.GetSize().y;
    float factor = std::floor(std::max(factor_x, factor_y));
    Camera::zoom = factor;
	
    start->box.Move({ Game::GetInstance().GetWindowSize().x / (Camera::zoom * 2), Game::GetInstance().GetWindowSize().y / (Camera::zoom * 2) });
    sr->SetCameraFollower(true);
    start->AddComponent(sr);
    this->AddObject(start);

    //std::shared_ptr<GameObject> text =  std::make_shared<GameObject>();
    //std::shared_ptr<Text> textComponent =  std::make_shared<Text>(*text, "resources/font/neodgm.ttf", 20, Text::SOLID, "Press SPACEBAR to play again or ESC to leave",SDL_Color{255,255,255}, 1, true);
    
    //text->AddComponent(textComponent);
    //this->AddObject(text);
    //text->box.Move({Game::GetInstance().GetWindowSize().x/2, Game::GetInstance().GetWindowSize().y/2});
    //text->z = 1;


    {
        started = true;
        m_Particle.SizeBegin = 4.0f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 1.0f;
        m_Particle.LifeTime = 1.0f;
        m_Particle.Velocity = { 0.0f, 0.0f };
        m_Particle.VelocityVariation = { 10.0f, 10.0f };
        m_Particle.Position = { 0.0f, 0.0f };
        m_Particle.VelocityFunction = { [](float x) {return 1 - (1 - x) * (1 - x); },[](float x) {return 1 - (1 - x) * (1 - x); } };
    }
    std::shared_ptr<GameObject> particles =  std::make_shared<GameObject>();
    std::shared_ptr<ParticleSystem> ps =  std::make_shared<ParticleSystem>(*particles, m_Particle);
    
    particles->AddComponent(ps);
    particles->box.Move({(Camera::pos.x+Game::GetInstance().GetWindowSize().x/2)-50, (Camera::pos.y+Game::GetInstance().GetWindowSize().y/2)-50});
    particles->z = 1;
    particlesSystem = this->AddObject(particles);
}
TitleState::~TitleState()
{
	m_backgroundMusic.Stop();
    this->objectArray.clear();
}
void TitleState::LoadAssets() {
}
void TitleState::Update(float dt)
{
    menu->Update(dt);

    InputManager &ip = InputManager::GetInstance();

    if (ip.KeyPress(ESCAPE_KEY) || ip.QuitRequested() || SDL_QuitRequested())
    {
        quitRequested = true;
    }
    if (ip.KeyPress(SPACE_KEY))
    {
        popRequested = true;
        Game::GetInstance().Push("Stage");
    }
    if(auto ps = particlesSystem.lock()){
        //TODO: rever questão do sistema de particula estar contido em um gameObject
        ps->box.Move({ip.GetMouseX(), ip.GetMouseY()});
        if(auto particles = std::dynamic_pointer_cast<ParticleSystem>(ps->GetComponent("ParticleSystem").lock())){
            particles->SetExplosiveness(1);
            particles->SetOneshot(true);
            particles->Play();
        }
        

    }
    UpdateArray(dt);

}
void TitleState::Render()
{
    RenderArray();
    menu->Render();
}

void TitleState::Start()
{
    menu->Start();
	menu->enabled = true;
	menu->SetCurrentState(MenuStates::START);

    LoadAssets();
    StartArray();
    if(auto ps = particlesSystem.lock()){
        if(auto particles = std::dynamic_pointer_cast<ParticleSystem>(ps->GetComponent("ParticleSystem").lock())){
            particles->SetAmount(30);
        }
    }
    m_backgroundMusic.Play();

}

void TitleState::Resume(){}
void TitleState::Pause(){}