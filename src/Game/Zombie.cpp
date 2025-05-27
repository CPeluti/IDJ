#include "Game/Zombie.h"
#include "Game/Bullet.h"
#include "Game/Character.h"
#include "Game/Lifebar.h"
#include "Game/HealthSystem.h"
#include "Game/SlowMotionEffect.h"
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

int Zombie::zombieCounter = 0;

Zombie::Zombie(GameObject &associated) : Component(associated),
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

    SpriteRenderer *srZombie = new SpriteRenderer(associated, "resources/img/Enemy.png", 3, 2);
    associated.AddComponent(srZombie);
    Collider *collider = new Collider(associated, {"layer0"}, new OnCollisionEvent(associated));
    associated.AddComponent(collider);
    Collider *interactionCollider = new Collider(associated, {"interaction0"}, new OnInteractionEvent(associated, InteractionType::None), {100, 100});
    associated.AddComponent(interactionCollider);

    Animator *animator = new Animator(associated);
    animator->AddAnimation("walking", new Animation(0, 3, 0.3));
    animator->AddAnimation("r_walking", new Animation(0, 3, 0.3, SDL_FLIP_HORIZONTAL));
    animator->AddAnimation("dead", new Animation(5, 5, 0));
    animator->AddAnimation("hit", new Animation(4, 4, 0));
    animator->AddAnimation("r_hit", new Animation(4, 4, 0, SDL_FLIP_HORIZONTAL));
    associated.AddComponent(animator);

    HealthSystem *healthSystem = new HealthSystem(associated, hitpoints);
    associated.AddComponent(healthSystem);

    // Lifebar *l = new Lifebar(associated,(int)hitpoints, {associated.box.GetSize().x, (float)10},{0,(int)associated.box.GetSize().y/4});
    // l->setAmount(hitpoints);
    // associated.AddComponent(l);

    associated.box.Move({600, 450});

    animator->SetAnimation("walking");

    m_Effects.push_back(std::make_unique<SlowMotionEffect>(0.5, 2));

    zombieCounter++;
}

bool Zombie::OnDamageTaken(OnDamageTakenEvent &evt)
{
    Animator *animator = (Animator *)associated.GetComponent("Animator");
    // Lifebar *l = (Lifebar *)associated.GetComponent("Lifebar");
    // l->setAmount(hitpoints);

    damageSound.Play(1);
    hit = true;

    hitTimer.Restart();
    if (flip)
        animator->SetAnimation("r_hit");
    else
        animator->SetAnimation("hit");
    if (((HealthSystem *)associated.GetComponent("HealthSystem"))->GetHp() <= 0 && !isDead)
    {
        this->associated.RemoveComponent(this->associated.GetComponent("Collider"));
        // this->associated.RemoveComponent(l);
        isDead = true;
        deathTimer.Restart();
        deathSound.Play(1);
        animator->SetAnimation("dead");
    }
    return true;
}

bool checkClickInsideBox(int x, int y, float boxX, float boxY, float boxW, float boxH)
{
    return (x > boxX && x < boxX + boxW) && (y > boxY && y < boxY + boxH);
}

Zombie::~Zombie()
{
    zombieCounter--;
}
void Zombie::Start() {}

void Zombie::Update(float dt)
{
    Animator *animator = ((Animator *)associated.GetComponent("Animator"));
    // this->Damage(1);
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
        if (!hit && Character::player != nullptr)
        {
            Vec2 playerPos = Character::player->GetPos();
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
            }
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

bool Zombie::Is(std::string type)
{
    return type == "Zombie";
}

void Zombie::Render() {}

int Zombie::GetDamage()
{
    return damage;
}

void Zombie::OnEvent(Event &evt)
{
    EventDispatcher dispatcher(evt);

    dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Zombie::OnCollision));
    dispatcher.Dispatch<OnDamageTakenEvent>(BIND_EVENT_FN(Zombie::OnDamageTaken));
    dispatcher.Dispatch<OnInteractionEvent>(BIND_EVENT_FN(Zombie::OnInteraction));
}

bool Zombie::OnCollision(OnCollisionEvent &evt)
{
    GameObject &go = evt.GetGameObject();
    OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->damage);

    if (go.GetComponent("HealthSystem"))
        go.subject.notify(e);

    return true;
}

bool Zombie::OnInteraction(OnInteractionEvent &evt)
{
    LOG_INFO("Zombie interaction event received");
    GameObject &target = evt.GetGameObject();
    switch (evt.GetInteractionType())
    {
    case InteractionType::Effect:
    {
        std::vector<std::weak_ptr<Effect>> effects;
        for (auto &effect : this->m_Effects)
        {
            std::weak_ptr<Effect> newWeakEffect = effect;
            effects.push_back(newWeakEffect);
        }
        OnEffectEvent e = OnEffectEvent(effects);
        target.subject.notify(e);
        break;
    }

    default:
        break;
    }
    return true;
}
