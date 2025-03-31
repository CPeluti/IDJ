#include <memory>
#include "Core/Game.h"
#include "Core/Component.h"
#include "Core/Character.h"
#include "Core/Camera.h"
#include "Core/Bullet.h"
#include "Core/Lifebar.h"
#include "Core/Zombie.h"
#include "Core/Gun.h"
#include "Core/SpriteRenderer.h"
#include "Core/PlayerController.h"
#include "Core/Animator.h"
#include "Core/Collider.h"
#include "Core/HealthSystem.h"

Character *Character::player = nullptr;
int Character::npcCounter = 0;
Character::Character(GameObject &associated, std::string sprite, bool isPlayer) : Component(associated),
                                                                   gun(),
                                                                   taskQueue(),
                                                                   speed{1, 1},
                                                                   linearSpeed(300),
                                                                   hp(500),
                                                                   isDead(false),
                                                                   deathTimer(5),
                                                                   extraProjectiles(3)
{
    this->associated.subject.addObserver(this);

    SpriteRenderer *sr = new SpriteRenderer(associated, sprite, 3, 4);
    Animator *animator = new Animator(associated);
    Collider *collider = new Collider(associated);
    HealthSystem *hs = new HealthSystem(associated, hp);

    if(!isPlayer){
        Character::npcCounter++;
    } else {
        PlayerController *playerController = new PlayerController(associated);
        associated.AddComponent(playerController);
    }


    // Lifebar *l = new Lifebar(associated,(int)hp, {associated.box.GetSize().x, (float)10},{0,(int)associated.box.GetSize().y/4});
    // l->setAmount(hp);

    associated.AddComponent(sr);
    associated.AddComponent(animator);
    associated.AddComponent(collider);
    associated.AddComponent(hs);

    // associated.AddComponent(l);


    animator->AddAnimation("walking", new Animation(0, 5, 0.2));
    animator->AddAnimation("idle", new Animation(6, 9, 0.5));
    animator->AddAnimation("i_walking", new Animation(0, 5, 0.2, SDL_FLIP_HORIZONTAL));
    animator->AddAnimation("i_idle", new Animation(6, 9, 0.5, SDL_FLIP_HORIZONTAL));
    animator->AddAnimation("dead", new Animation(10, 11, 0.5));
    animator->SetAnimation("idle");
    flip = false;

}

Character::~Character()
{
    if(Character::player != this){
        Character::npcCounter--;
    } else {
        Character::player = nullptr;
    }
}

void Character::Start()
{
    State &s = Game::GetInstance().GetCurrentState();
    GameObject *gunObj = new GameObject();
    Gun *gunComponent = new Gun(*gunObj, s.GetObjectPtr(&associated));

    gunObj->AddComponent(gunComponent);

    this->gun = s.AddObject(gunObj);

}
// void Character::Damage(int amount){
//     hp -= amount;
//     Lifebar *l = (Lifebar *)associated.GetComponent("Lifebar");
//     Animator *animator = (Animator *)associated.GetComponent("Animator");
//     animator->SetAnimation("hit");
//     // subject.notify(*this, Observer::Event::onTakeDamage);
//     if (hp <= 0 && !isDead)
//     {
//         if(auto g = this->gun.lock()){
//             g->RequestDelete();
//         }
//         associated.RemoveComponent(l);
//         isDead = true;
//         deathTimer.Restart();
//         animator->SetAnimation("dead");
//         if(Character::player == this){
//             Camera::Unfollow();
//         }
//     }
// }
void Character::Update(float dt)
{
    Animator *animator = ((Animator *)associated.GetComponent("Animator"));
    if (isDead)
    {
        deathTimer.Update(dt);
        if (deathTimer.Expired())
        {
            associated.RequestDelete();
        }
        return;
    }
    if (taskQueue.size() == 0 && animator)
    {
        animator->SetAnimation(flip ? "idle" : "i_idle");
    }
    while (taskQueue.size() > 0)
    {
        speed = {0, 0};
        Command c = taskQueue.front();
        switch (c.type)
        {
            case c.MOVE:
            {
                speed = c.pos.normalized() * linearSpeed;
            }
            break;

            case c.SHOOT:
            {
                if (auto g = gun.lock())
                {
                    ((Gun *)g->GetComponent("Gun"))->Shoot(c.pos);
                }
            }
            break;
        }
        taskQueue.pop();
        if (speed.x || speed.y)
        {
            animator->SetAnimation(flip ? "walking" : "i_walking");
            Vec2 newSpeed = (speed * dt);
            Vec2 currentPos = associated.box.GetPos();
            if(this == this->player){
                Vec2 charPos = this->associated.box.GetPos();
                if(charPos.x < 640 && newSpeed.x < 0){
                    newSpeed.x = 0;
                } 
                else if (charPos.x > 1920 -associated.box.GetSize().x && newSpeed.x > 0)
                {
                    newSpeed.x = 0;
                }
                if(charPos.y < 512 && newSpeed.y < 0){
                    newSpeed.y = 0;
                } else if (charPos.y > 2048-associated.box.GetSize().y  && newSpeed.y > 0){
                    newSpeed.y = 0;
                }
            }
            associated.box.RawMove(currentPos + newSpeed);
        }
    }
}

void Character::Render() {}

Character::Command::Command(CommandType type, Vec2 pos) : type(type), pos(pos) {}

void Character::OnEvent(Event& e){
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Character::OnCollision));
}

bool Character::OnCollision(OnCollisionEvent& evt){
    // GameObject& go = evt.GetGameObject();
    // Bullet *b = (Bullet *)go.GetComponent("Bullet");
    // Zombie *z = (Zombie *)go.GetComponent("Zombie");
    // if (b != nullptr && ((Character::player == this && b->targetsPlayer) || (Character::player != this)))
    // {
    //     associated.subject.notify(OnDamageTakenEvent(associated, 100))
    // }
    // if (z != nullptr && !z->isDead && (Character::player == this))
    // {
    //     // Damage(z->GetDamage());
    // }
    return true;
}