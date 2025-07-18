#include "Core/Game.h"
#include "Core/Camera.h"

#include "Game/WaveSpawner.h"
#include "Game/Enemy.h"
#include "Game/Boss.h"
#include "Game/Character.h"
#include "Game/AIController.h"

WaveSpawner::WaveSpawner(GameObject &associated) : Component(associated), enemyCounter(0), currentWave(4), bossCounter(0), bossSpawned(false), win(false), lose(false)
{
    for (int i = 0; i < 5; i++)
    {
		LOG_INFO("WaveSpawner: Creating wave {}", i);
        Wave* w = new Wave(1 * (i + 1) * (i + 1), 1);
         //w->cooldown.Restart();
        waves.push_back(w);
    }
}

void WaveSpawner::Start()
{

	std::shared_ptr<SpriteRenderer> waveBar = std::make_shared<SpriteRenderer>(this->associated, "resources/img/barraprogresso.png",1,1);
    waveBar->SetCameraFollower(true);
	this->associated.AddComponent(waveBar);
    Vec2 pos = Vec2{((Game::GetInstance().GetWindowSize() / 2) / Camera::zoom).x, (Game::GetInstance().GetWindowSize().y)/Camera::zoom-this->associated.box.GetSize().y-5};
    this->associated.box.Move(pos);
}

void WaveSpawner::Update(float dt)
{
    //if(waves.size()<=(unsigned int)currentWave){
    //    this->associated.RequestDelete();
    //    return;
    //}
    if(Character::player.lock() == nullptr){
        LOG_INFO("Player is dead, wave spawner will not spawn enemies");
        lose = true;
        return;
	}
    float randomAngle = rand() % 36000 / 100.0;
    Vec2 windowSize = Game::GetInstance().GetWindowSize();
    Vec2 distance = { 150.0, .0 };
    if(currentWave >= (int)waves.size() && !bossSpawned){
        std::shared_ptr<GameObject> enemy = std::make_shared<GameObject>();
        std::shared_ptr<Boss> enemyComponent = std::make_shared<Boss>(*enemy, "resources/img/Sprite-Shoggoth-All.png", 40, 1);
        //enemy->box.Move({ ip.GetMouseX(), ip.GetMouseY() });
        enemy->AddComponent(enemyComponent);
        if (auto s = Game::GetInstance().GetCurrentState())
        {
            s->AddObject(enemy);
        }
        if (auto character = Character::player.lock()) {
            Vec2 targetPos = character->GetPos() + Vec2::Rotate({ distance }, randomAngle);
            Raycast r = this->associated.CastRaycast(character->GetPos(), targetPos, 2000, 1);
            LOG_INFO("pos: {} targetpos: {}", character->GetPos(), targetPos);
            LOG_INFO(r.intersects);
            LOG_INFO(r.intersectionPoint);
            LOG_INFO(r.maxDistanceExceeded);
            if (r.intersects) {
                enemy->box.Move(r.intersectionPoint + enemy->box.GetSize());
            }
            else {
                enemy->box.Move(targetPos);
            }
        }
		bossSpawned = true;
        bossCounter++;
	}
    else if (!bossSpawned) {

        waves[currentWave]->cooldown.Update(dt);
        if (waves[currentWave]->cooldown.Expired())
        {       
            float randomAngle = rand() % 36000 / 100.0;
            Vec2 windowSize = Game::GetInstance().GetWindowSize();
            Vec2 distance = { (windowSize.x / 2)/Camera::zoom, .0 };

            if(auto s = std::move(Game::GetInstance().GetCurrentState())){
                if (enemyCounter <= waves[currentWave]->enemies)
                {
                    std::shared_ptr<GameObject> enemy = std::make_shared<GameObject>();
                    std::shared_ptr<Enemy> enemyComponent = std::make_shared<Enemy>(*enemy, "resources/img/Axolote.png", 8, 7);
                    //std::shared_ptr<Enemy> z = std::make_shared<Enemy>(*go);
                    enemy->AddComponent(enemyComponent);
                    s->AddObject(enemy);
                    if (auto character = Character::player.lock()) {
                        Vec2 targetPos = character->GetPos() + Vec2::Rotate({ distance }, randomAngle);
                        Raycast r = this->associated.CastRaycast(targetPos, character->GetPos(), 2000, 1);
                        LOG_INFO("pos: {} targetpos: {}", character->GetPos(), targetPos);
                        LOG_INFO(character->GetPos());
                        LOG_INFO(r.intersectionPoint);
                        LOG_INFO(randomAngle);
                        if (r.intersects) {
					        enemy->box.Move(r.intersectionPoint + enemy->box.GetSize());
                        }
                        else {
                            enemy->box.Move(targetPos);
                        }
                        enemyCounter++;
                    }
                }
            }
            waves[currentWave]->cooldown.Restart();
        }

        if (enemyCounter >= waves[currentWave]->enemies && Enemy::enemyCounter ==0 && !bossSpawned)
        {
            currentWave++;
            enemyCounter = 0;
        }

    }
    if (bossSpawned && Boss::bossCounter < 1)
    {
        LOG_INFO("WIN");
        if(waves.size()<=(unsigned int)currentWave){
            //this->associated.RequestDelete();
            win = true;
            return;
        }
	}
}

bool WaveSpawner::Win() {
    return win;
}

bool WaveSpawner::Lose() {
    return lose;
}
void WaveSpawner::Render() {

    SDL_Rect barToFill = { this->associated.box.GetPos().x, this->associated.box.GetPos().y, (int)(this->associated.box.GetSize().x), (int)(this->associated.box.GetSize().y)};
    SDL_Rect barBg = { this->associated.box.GetPos().x, this->associated.box.GetPos().y, (int)(this->associated.box.GetSize().x), (int)(this->associated.box.GetSize().y) };
    //SDL_Rect bg = { lifebar.x, lifebar.y, (int)(lifebar.w), (int)(lifebar.h) };
    //barToFill.x = lifebar.x + (lifebar.w / 2) - (barToFill.w / 2);
    //barToFill.y = lifebar.y + (lifebar.h / 2) - (barToFill.h / 2);
    barToFill.w = barToFill.w * currentWave / waves.size();
    barToFill.x *= Camera::zoom;
    barToFill.y *= Camera::zoom;
    barToFill.w *= Camera::zoom;
    barToFill.h *= Camera::zoom;

    barBg.x *= Camera::zoom;
    barBg.y *= Camera::zoom;
    barBg.w *= Camera::zoom;
    barBg.h *= Camera::zoom;



    //GPU_RectangleFilled(Game::GetInstance().GetGPUTarget(), bg.x, bg.y, bg.x + bg.w, bg.y + bg.h, { 0, 0, 0, 255 });
    // SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, 255);
    GPU_RectangleFilled(Game::GetInstance().GetGPUTarget(), barBg.x, barBg.y, barBg.x + barBg.w, barBg.y + barBg.h, { 136,4,52,255 });
    GPU_RectangleFilled(Game::GetInstance().GetGPUTarget(), barToFill.x, barToFill.y, barToFill.x + barToFill.w, barToFill.y + barToFill.h, { 255,255,255,255});


}

bool WaveSpawner::Is(std::string type)
{
    return type == "WaveSpawner";
}