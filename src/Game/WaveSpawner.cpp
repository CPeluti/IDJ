#include "Core/Game.h"
#include "Core/Camera.h"

#include "Game/WaveSpawner.h"
#include "Game/Zombie.h"
#include "Game/Character.h"
#include "Game/AIController.h"

WaveSpawner::WaveSpawner(GameObject &associated) : Component(associated), zombieCounter(0), npcCounter(0), currentWave(0)
{
    for (int i = 0; i < 2; i++)
    {
        Wave *w = new Wave(1, 0, 1);
        // w->cooldown.Restart();
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

    if (waves[currentWave]->cooldown.Expired())
    {
        
        float randomAngle = rand() % 36000 / 100.0;
        Vec2 windowSize = Game::GetInstance().GetWindowSize();
        Vec2 distance = {windowSize.x / 2, .0};

        if(auto s = std::move(Game::GetInstance().GetCurrentState())){
            if (zombieCounter < waves[currentWave]->zombies)
            {
                std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
                std::shared_ptr<Zombie> z = std::make_shared<Zombie>(*go);
                go->AddComponent(z);
                s->AddObject(go);
                go->box.Move(Camera::pos + windowSize / 2 + Vec2::Rotate({distance}, randomAngle));
                zombieCounter++;
            }
            if(npcCounter < waves[currentWave]->npcs){
                randomAngle = rand() % 36000 / 100.0;
                std::shared_ptr<GameObject> cgo = std::make_shared<GameObject>();
                std::shared_ptr<Character> c = std::make_shared<Character>(*cgo, "resources/img/NPC.png");
                std::shared_ptr<AIController> a = std::make_shared<AIController>(*cgo);
                cgo->AddComponent(c);
                cgo->AddComponent(a);
                s->AddObject(cgo);
                cgo->box.Move(Camera::pos + windowSize / 2 + Vec2::Rotate({distance}, randomAngle));
                npcCounter++;
            }
        }




        // if(zombieCounter)
        waves[currentWave]->cooldown.Restart();
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