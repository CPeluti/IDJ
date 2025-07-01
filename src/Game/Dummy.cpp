#include "Game/Dummy.h"
#include "Game/Bullet.h"
#include "Game/Character.h"
#include "Game/Lifebar.h"
#include "Game/HealthSystem.h"
#include "Core/GameObject.h"
#include "Core/Animation.h"
#include "Core/SpriteRenderer.h"
#include "Core/Animator.h"
#include "Core/InputManager.h"
#include "Core/Game.h"
#include "Core/Camera.h"
#include "Core/Collider.h"
#include "Core/Log.h"
#include <iostream>

int Dummy::dummyCounter = 0;

Dummy::Dummy(GameObject &associated) : Component(associated),
                                         isDead(false),
                                         hitpoints(600),
                                         damageSound("resources/audio/Hit1.wav"),
                                         deathSound("resources/audio/Dead.wav"),
                                         hit(false),
                                         damage(50),
                                         hitTimer(0.5),
                                         deathTimer(5)
{
    this->associated.subject.addObserver(this);

    std::shared_ptr<SpriteRenderer> srDummy = std::make_shared<SpriteRenderer>(associated, "resources/img/Enemy.png", 3, 2);
    std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "Dummy", new OnCollisionEvent(associated), Vec2{100,100});
    //std::shared_ptr<Collider> interactionCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, new OnInteractionEvent(associated, InteractionType::None), Vec2{ 100, 100 });
    std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);
    std::shared_ptr<HealthSystem> healthSystem = std::make_shared<HealthSystem>(associated, hitpoints);
    associated.AddComponent(srDummy);
    associated.AddComponent(collider);
    //associated.AddComponent(interactionCollider);

    animator->AddAnimation("walking", new Animation(0, 3, 0.3));
    animator->AddAnimation("r_walking", new Animation(0, 3, 0.3, SDL_FLIP_HORIZONTAL));
    animator->AddAnimation("dead", new Animation(5, 5, 0));
    animator->AddAnimation("hit", new Animation(4, 4, 0));
    animator->AddAnimation("r_hit", new Animation(4, 4, 0, SDL_FLIP_HORIZONTAL));
    associated.AddComponent(animator);

    associated.AddComponent(healthSystem);

    // Lifebar *l = new Lifebar(associated,(int)hitpoints, {associated.box.GetSize().x, (float)10},{0,(int)associated.box.GetSize().y/4});
    // l->setAmount(hitpoints);
    // associated.AddComponent(l);

    associated.box.Move({600, 450});

    animator->SetAnimation("walking");

    //m_Effects.push_back(std::make_unique<SlowMotionEffect>(0.5, 2));

    dummyCounter++;
}

bool Dummy::OnDamageTaken(OnDamageTakenEvent &evt)
{
    if(auto animator = std::dynamic_pointer_cast<Animator>(this->associated.GetComponent("Animator").lock())){


        damageSound.Play(1);
        hit = true;

        hitTimer.Restart();
        if (flip)
            animator->SetAnimation("r_hit");
        else
            animator->SetAnimation("hit");
        if(auto hs = std::dynamic_pointer_cast<HealthSystem>(this->associated.GetComponent("HealthSystem").lock())){
            
            if (hs->GetHp() <= 0 && !isDead)
            {
                this->associated.RemoveComponent(this->associated.GetComponent("Collider"));
                // this->associated.RemoveComponent(l);
                isDead = true;
                deathTimer.Restart();
                deathSound.Play(1);
                animator->SetAnimation("dead");
            }
        }
    }
    return true;
}

//bool checkClickInsideBox(int x, int y, float boxX, float boxY, float boxW, float boxH)
//{
//    return (x > boxX && x < boxX + boxW) && (y > boxY && y < boxY + boxH);
//}

Dummy::~Dummy()
{
    dummyCounter--;
}
void Dummy::Start() {}

void Dummy::Update(float dt)
{
    if(auto animator = std::dynamic_pointer_cast<Animator>(associated.GetComponent("Animator").lock()))
    {// this->Damage(1);
        hitTimer.Update(dt);
        if (isDead)
        {

            deathTimer.Update(dt);
            if (deathTimer.Expired())
            {
                associated.RequestDelete();
            }
        }
        else
        {
            if (!hit && Character::player.lock())
            {
               /* Vec2 playerPos = Character::player.lock()->GetPos();
                Vec2 distance = playerPos - associated.box.center();
                Vec2 currentPos = this->associated.box.center();
                this->associated.box.Move(currentPos + distance.normalized() * dt * 200);
                flip = distance.x < 0;
                if (flip)
                {
                    animator->SetAnimation("r_walking");
                }
                else
                {
                    animator->SetAnimation("walking");
                }*/
            }
            if (hit && hitTimer.Expired() && !isDead)
            {
                if (flip)
                {
                    animator->SetAnimation("r_walking");
                }
                else
                {
                    animator->SetAnimation("walking");
                }

                hit = false;
            }
        }
    }
}

bool Dummy::Is(std::string type)
{
    return type == "Dummy";
}

void Dummy::Render() {}

int Dummy::GetDamage()
{
    return damage;
}

void Dummy::OnEvent(Event& evt)
{
    EventDispatcher dispatcher(evt);

    dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Dummy::OnCollision));
    dispatcher.Dispatch<OnDamageTakenEvent>(BIND_EVENT_FN(Dummy::OnDamageTaken));
    dispatcher.Dispatch<OnInteractionEvent>(BIND_EVENT_FN(Dummy::OnInteraction));
}

bool Dummy::OnCollision(OnCollisionEvent& evt)
{
    GameObject& go = evt.GetGameObject();
    OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->damage);

    if (go.GetComponent("HealthSystem").lock())
        go.subject.notify(e);

    return true;
}

bool Dummy::OnInteraction(OnInteractionEvent &evt)
{
    GameObject &target = evt.GetGameObject();
    switch (evt.GetInteractionType())
    {
    case InteractionType::Effect:
    {
        std::vector<std::weak_ptr<Effect<Entity>>> effects;
        for (auto &effect : this->m_Effects)
        {
            std::weak_ptr<Effect<Entity>> newWeakEffect = effect;
            effects.push_back(newWeakEffect);
        }
        OnEffectEvent<Entity> e = OnEffectEvent<Entity>(effects);
        target.subject.notify(e);
        break;
    }

    default:
        break;
    }
    return true;
}
