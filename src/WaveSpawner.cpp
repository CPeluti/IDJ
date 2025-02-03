#include "WaveSpawner.h"
#include "Zombie.h"
#include "Game.h"
#include "Camera.h"

WaveSpawner::WaveSpawner(GameObject& associated): Component(associated), zombieCounter(0), currentWave(0){
    for(int i = 0; i<10; i++){
        Wave* w = new Wave(10, 10);
        w->cooldown.Restart();
        waves.push_back(w);
    }
}

void WaveSpawner::Update(float dt){
    waves[currentWave]->cooldown.Update(dt);
    if(zombieCounter < waves[currentWave]->zombies){
        if(waves[currentWave]->cooldown.Expired()){
            float randomAngle = rand()%36000/100.0;
            GameObject* go = new GameObject();
            Zombie* z = new Zombie(*go);
            go->AddComponent(z);
            Game::GetInstance().GetState().AddObject(go);
            Vec2 windowSize = Game::GetInstance().GetWindowSize();
            Vec2 distance = {windowSize.x/2,.0};
            go->box.Move(Camera::pos + windowSize/2 + Vec2::Rotate({distance},randomAngle));
            // if(zombieCounter)
            zombieCounter++;
            waves[currentWave]->cooldown.Restart();

        }
    }
}
void WaveSpawner::Render(){}

bool WaveSpawner::Is(std::string type){
    return type == "WaveSpawner";
}