#include "WaveSpawner.h"
#include "Zombie.h"
#include "Character.h"
#include "Game.h"
#include "Camera.h"
#include "AIController.h"

WaveSpawner::WaveSpawner(GameObject &associated) : Component(associated), zombieCounter(0), npcCounter(0), currentWave(0)
{
    for (int i = 0; i < 10; i++)
    {
        Wave *w = new Wave(1, 1, 1);
        w->cooldown.Restart();
        waves.push_back(w);
    }
}

void WaveSpawner::Update(float dt)
{
    if(waves.size()<=(unsigned int)currentWave){
        this->associated.RequestDelete();
        return;
    }
    waves[currentWave]->cooldown.Update(dt);
    if (zombieCounter < waves[currentWave]->zombies && npcCounter < waves[currentWave]->npcs)
    {
        if (waves[currentWave]->cooldown.Expired())
        {
            float randomAngle = rand() % 36000 / 100.0;
            Vec2 windowSize = Game::GetInstance().GetWindowSize();
            Vec2 distance = {windowSize.x / 2, .0};

            GameObject *go = new GameObject();
            Zombie *z = new Zombie(*go);
            go->AddComponent(z);
            Game::GetInstance().GetCurrentState().AddObject(go);
            go->box.Move(Camera::pos + windowSize / 2 + Vec2::Rotate({distance}, randomAngle));

            randomAngle = rand() % 36000 / 100.0;
            GameObject* cgo = new GameObject();
            Character* c = new Character(*cgo, "resources/img/NPC.png");
            AIController* a = new AIController(*cgo);
            cgo->AddComponent(c);
            cgo->AddComponent(a);
            Game::GetInstance().GetCurrentState().AddObject(cgo);
            cgo->box.Move(Camera::pos + windowSize / 2 + Vec2::Rotate({distance}, randomAngle));




            // if(zombieCounter)
            npcCounter++;
            zombieCounter++;
            waves[currentWave]->cooldown.Restart();
        }
    }
    else
    {
        if (Zombie::zombieCounter == 0 && Character::npcCounter == 0)
        {
            currentWave++;
            zombieCounter = 0;
            npcCounter = 0;
        }
    }
}
void WaveSpawner::Render() {}

bool WaveSpawner::Is(std::string type)
{
    return type == "WaveSpawner";
}