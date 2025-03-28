#include "Gun.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "InputManager.h"
#include "Character.h"
#include "Game.h"
#include "Bullet.h"
#include <iostream>
#include <cmath>

#define OFFSET 70
Gun::Gun(GameObject &associated, std::weak_ptr<GameObject> character) : Component(associated),
                                                                        shotSound("resources/audio/Range.wav"),
                                                                        reloadSound("resources/audio/PumpAction.mp3"),
                                                                        cooldown(0.1),
                                                                        cdTimer(cooldown),
                                                                        character(character),
                                                                        angle(0),
                                                                        bulletOutput(0, 0),
                                                                        projectileAmount(3)
{
    SpriteRenderer *sr = new SpriteRenderer(associated, "resources/img/Gun.png", 3, 2);
    Animator *animator = new Animator(associated);

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
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    InputManager &ip = InputManager::GetInstance();

    bool inverted = true;
    if (auto c = character.lock())
    {
        Character* charComponent = ((Character*)c->GetComponent("Character"));
        Vec2 centroChar = c->box.center();
        Vec2 target;
        if(charComponent == Character::player){
            target = {(float)ip.GetMouseX(), (float)ip.GetMouseY()};

        } else if (charComponent != Character::player && Character::player != nullptr){
            target = Character::player->GetPos();
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
            charComponent->flip=false;
        }
        else
        {
            inverted = true;
            charComponent->flip=true;
        }
        associated.box.RawMove(centroChar);

        Vec2 pos = {OFFSET, 0};
        Vec2 currentPos = associated.box.GetPos();

        pos = Vec2::Rotate(pos, angle);

        associated.box.Move(currentPos + pos);

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
            Character* character = (Character*)c->GetComponent("Character");
            int projectiles = (projectileAmount+character->getProjectileNumber());
            float angleStep = 10;
            float startingAngle = Vec2::Angle(centro, target);
            angle = startingAngle-((projectiles/2)*angleStep);

            // int offset = 10;
            // int startingPoint = -((projectileAmount/2) * 10);
            for(int i = 0; i<projectiles; i++){
                GameObject *bullet = new GameObject();
                Bullet *bulletComponent = new Bullet(*bullet, angle, 350, 500, 400, Character::player != this->associated.GetComponent("Character"));
                Vec2 gunOffset = {associated.box.GetSize().x + OFFSET, .0};
                Vec2 bulletOffset = Vec2::Rotate(gunOffset, angle);
                bullet->box.Move(centro + bulletOffset);
    
                // bullet->box.Move(centro);
    
                bullet->AddComponent(bulletComponent);
                bullet->angleDeg = angle + 90;
                Game::GetInstance().GetCurrentState().AddObject(bullet);
                angle+=angleStep;
            }
            associated.angleDeg = angle;
            shotSound.Play();
            cdTimer.Restart();
        }
    }
}

bool Gun::Is(std::string type)
{
    return type == "Gun";
}

void Gun::Render() {}