#include "Core/SpriteRenderer.h"
#include "Core/Animator.h"
#include "Core/InputManager.h"
#include "Core/Game.h"
#include "Core/Log.h"
#include <iostream>
#include <cmath>

#include "Game/Gun.h"
#include "Game/Character.h"
#include "Game/Bullet.h"
#include "Game/Spell.h"
#define OFFSET 70
Gun::Gun(GameObject &associated, std::weak_ptr<GameObject> character) : Component(associated),
                                                                        shotSound("resources/audio/Range.wav"),
                                                                        reloadSound("resources/audio/PumpAction.mp3"),
                                                                        cooldown(0.1),
                                                                        cdTimer(cooldown),
                                                                        character(character),
                                                                        angle(0),
                                                                        bulletOutput(0, 0),
                                                                        projectileAmount(1)
{
    std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, "resources/img/Gun.png", 3, 2);
    std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);

    associated.AddComponent(sr);
    associated.AddComponent(animator);
    animator->AddAnimation("idle", new Animation(0, 0, 0));
    animator->AddAnimation("reloading", new Animation(1, 5, cooldown / 5.0));
    animator->AddAnimation("i_idle", new Animation(0, 0, 0, SDL_FLIP_VERTICAL));
    animator->AddAnimation("i_reloading", new Animation(1, 5, cooldown / 5.0, SDL_FLIP_VERTICAL));

    animator->SetAnimation("idle");
}

void Gun::Update(float dt)
{
    InputManager &ip = InputManager::GetInstance();

    bool inverted = true;
    if (auto c = character.lock())
    {
        if(auto charComponent = std::dynamic_pointer_cast<Character>(c->GetComponent("Character").lock())){
            Vec2 centroChar = c->box.center();
            Vec2 target;
            if(charComponent == Character::player.lock()){
                target = {(float)ip.GetMouseX(), (float)ip.GetMouseY()};

            } else if (charComponent != Character::player.lock() && Character::player.lock()){
                auto character = Character::player.lock();
                target = character->GetPos();
            } else {
                target = {0,0};
            }
            if (this->cdTimer.Expired())
            {
                angle = Vec2::Angle(centroChar, target);
                associated.angleDeg = angle;
            }

            if (angle >= 90 && angle <= 270)
            {
                inverted = false;
                charComponent->SetFlip(false);
            }
            else
            {
                inverted = true;
                charComponent->SetFlip(true);
            }
            associated.box.RawMove(centroChar);
            Vec2 pos = {OFFSET, 0};
            Vec2 currentPos = associated.box.GetPos();
            
            pos = Vec2::Rotate(pos, angle);
            associated.box.Move(currentPos + pos);
        }
        if(auto animator = std::dynamic_pointer_cast<Animator>(this->associated.GetComponent("Animator").lock())){
            if (cdTimer.Expired())
            {
                animator->SetAnimation(inverted ? "idle" : "i_idle");
            }
            else
            {
                animator->SetAnimation(inverted ? "reloading" : "i_reloading");
                if (cdTimer.GetTime() == 0)
                {
                    reloadSound.Play();
                }
                cdTimer.Update(dt);
            }
        }
    }
    else
    {
        associated.RequestDelete();
        return;
    }
}

void Gun::Shoot(Vec2 target)
{
    Vec2 centro;
    if (auto c = character.lock())
    {
        centro = c->box.center();
        if (cdTimer.Expired())
        {
            if(auto character = std::dynamic_pointer_cast<Character>(c->GetComponent("Character").lock())){
                //int projectiles = (projectileAmount+character->getProjectileNumber());
                //float angleStep = 10;
                //float startingAngle = Vec2::Angle(centro, target);
                //angle = startingAngle-((projectiles/2)*angleStep);

                //// int offset = 10;
                //// int startingPoint = -((projectileAmount/2) * 10);
                //for(int i = 0; i<projectiles; i++){
                //    std::shared_ptr<GameObject> bullet = std::make_shared<GameObject>();
                //    // Bullet *bulletComponent = new Bullet(*bullet, angle, 350, 500, 400, Character::player != this->associated.GetComponent("Character"));
                //    Vec2 gunOffset = {associated.box.GetSize().x + OFFSET, .0};
                //    Vec2 bulletOffset = Vec2::Rotate(gunOffset, angle);
                //    Vec2 bulletInitialPos = centro + bulletOffset;

                //    std::shared_ptr<FireAreaSpell> fSpell = std::make_shared<FireAreaSpell>(*bullet, bulletInitialPos);
                //    bullet->AddComponent(fSpell);
                //    // bullet->box.Move();
        
                //    // bullet->box.Move(bulletInitialPos);
                //    // bullet->AddComponent(bulletComponent);
                //    bullet->angleDeg = angle + 90;
                //    if(auto s = Game::GetInstance().GetCurrentState())
                //        s->AddObject(bullet);
                //    angle+=angleStep;
                //}
                //associated.angleDeg = startingAngle;
                //shotSound.Play();
                //cdTimer.Restart();
            }
        }
    }
}

bool Gun::Is(std::string type)
{
    return type == "Gun";
}

void Gun::Render() {}