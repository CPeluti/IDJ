#include "Core/Game.h"
#include "Core/Camera.h"

#include "Game/WaveSpawner.h"
#include "Game/Enemy.h"
#include "Game/Character.h"
#include "Game/AIController.h"

WaveSpawner::WaveSpawner(GameObject &associated) : Component(associated), enemyCounter(0), currentWave(0)
{
    for (int i = 0; i < 2; i++)
    {
        Wave *w = new Wave(1, 5);
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

    if (waves[currentWave]->cooldown.Expired())
    {
        
        float randomAngle = rand() % 36000 / 100.0;
        Vec2 windowSize = Game::GetInstance().GetWindowSize();
        Vec2 distance = {windowSize.x / 2, .0};

        if(auto s = std::move(Game::GetInstance().GetCurrentState())){
            if (enemyCounter <= waves[currentWave]->enemies)
            {
                std::shared_ptr<GameObject> enemy = std::make_shared<GameObject>();
                std::shared_ptr<Enemy> enemyComponent = std::make_shared<Enemy>(*enemy, "resources/img/Axolote.png", 8, 7);
                //std::shared_ptr<Enemy> z = std::make_shared<Enemy>(*go);
                enemy->AddComponent(enemyComponent);
                s->AddObject(enemy);
                Vec2 targetPos = Camera::pos + (windowSize / 2) + Vec2::Rotate({ distance }, randomAngle);
                Raycast r = this->associated.CastRaycast(Camera::pos + (windowSize / 2), targetPos, 300);
                if (r.intersects) {
					enemy->box.Move(r.intersectionPoint - enemy->box.GetSize());
                }
                else {
                    enemy->box.Move(targetPos);
                }
                enemyCounter++;
            }
        }




        // if(zombieCounter)
        waves[currentWave]->cooldown.Restart();
    }
    else
    {
        if (Enemy::enemyCounter == 0)
        {
            currentWave++;
            enemyCounter = 0;
        }
    }
}
void WaveSpawner::Render() {}

bool WaveSpawner::Is(std::string type)
{
    return type == "WaveSpawner";
}