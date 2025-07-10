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
    std::shared_ptr<Collider> collider = std::make_shared<Collider>(associated, std::vector<std::string>{"phys0"}, "Dummy", new OnCollisionEvent(associated), Vec2{100,100});
    //std::shared_ptr<Collider> interactionEffectCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"phys0"}, "entity", new OnInteractionEvent(associated, InteractionType::Effect), colliderSize, Vec2{ 1, 1 }, colliderOffset);
    //std::shared_ptr<Collider> interactionCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, new OnInteractionEvent(associated, InteractionType::None), Vec2{ 100, 100 });
    std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);
    std::shared_ptr<HealthSystem> healthSystem = std::make_shared<HealthSystem, GameObject&, int&>(associated, hitpoints);
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
                this->associated.RemoveComponent(this->associated.GetComponent("Collider").lock());
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
void Dummy::Start() {
    std::shared_ptr<GameObject> exclamation = std::make_shared<GameObject>();
    std::shared_ptr<SpriteRenderer> exclamationSprite = std::make_shared<SpriteRenderer>(*exclamation, "resources/img/exclamation.png", 6, 1);
    std::shared_ptr<Animator> exclamationAnimator = std::make_shared<Animator>(*exclamation, false);
    exclamationAnimator->AddAnimation("exclamation", new Animation(0, 5, 0.025));
    exclamation->AddComponent(exclamationSprite);
    exclamation->AddComponent(exclamationAnimator);
    Game::GetInstance().GetCurrentState()->AddObject(exclamation);
    this->exclamation = exclamation;
    exclamation->box.Move(this->associated.box.center() - Vec2{30, 30});
	exclamationSprite->enabled = false;

}

void Dummy::Update(float dt)
{
    //LOG_INFO("dummies: {}", dummyCounter);
    if (auto e = exclamation.lock()) {
		e->box.Move(this->associated.box.center() - Vec2{ 30, 30 });
    }
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
    if (auto c = Character::player.lock()) {
        Vec2 charPos = c->getAssociated()->box.center();
        auto raycast = this->associated.CastRaycast(charPos, this->associated.box.center(), 5000, 1);
        LOG_INFO("Raycast: {}", raycast);
        if (!raycast.intersects && !raycast.maxDistanceExceeded && !moving) {

            //moving = true;
            LOG_INFO("Character breadcrumb: {}", charPos);
            this->characterBreadcrumb = charPos;
            if (auto e = exclamation.lock()) {
                std::dynamic_pointer_cast<SpriteRenderer>(e->GetComponent("SpriteRenderer").lock())->enabled = true;
                if (!playerFound) {
                    std::dynamic_pointer_cast<Animator>(e->GetComponent("Animator").lock())->SetAnimation("exclamation");
                    playerFound = true;
                }
            }
        }
        else {
            if (auto e = exclamation.lock()) {
                std::dynamic_pointer_cast<SpriteRenderer>(e->GetComponent("SpriteRenderer").lock())->enabled = false;
                playerFound = false;
            }
        }
    }
  //  if (Vec2::Distance(this->characterBreadcrumb, this->associated.box.center())<10) {
		//this->associated.SetSpeed(Vec2::Zero);
  //      moving = false;
  //      if (auto e = exclamation.lock()) {
  //          std::dynamic_pointer_cast<SpriteRenderer>(e->GetComponent("SpriteRenderer").lock())->enabled = false;
		//}
  //  }
    else {
        if (this->characterBreadcrumb.x && this->characterBreadcrumb.y) {
            //this->associated.SetSpeed((this->characterBreadcrumb - this->associated.box.center()).normalized() * 100 * dt);
        }
    }
}

bool Dummy::Is(std::string type)
{
    return type == "Dummy";
}

void Dummy::Render() {
    if (auto c = Character::player.lock())
    {
        Vec2 charPos = (c->getAssociated()->box.center() * Camera::zoom) - Camera::pos;
        Vec2 dummyPos = (this->associated.box.center() * Camera::zoom) - Camera::pos;
        auto raycast = this->associated.CastRaycast(c->getAssociated()->box.center(), this->associated.box.center(), 5000, 1);
        if (!raycast.intersects && !raycast.maxDistanceExceeded) {
            GPU_Line(Game::GetInstance().GetGPUTarget(), charPos.x, charPos.y, dummyPos.x, dummyPos.y, { 255, 0, 0, 255 });
        }
    }
    GPU_Image* light = GPU_CreateImage(100, 100, GPU_FORMAT_RGBA);
    GPU_SetBlending(light, true);
    GPU_LoadTarget(light);

    GPU_ClearRGBA(light->target, 255, 0, 0, 155);

    float radius = 5* Camera::zoom / 2.0f;
    SDL_Color color = { 255,255,0,128 };


    GPU_CircleFilled(Game::GetInstance().GetGPUTarget(), (this->characterBreadcrumb.x * Camera::zoom) - Camera::pos.x, (this->characterBreadcrumb.y * Camera::zoom) - Camera::pos.y, radius, color);

}

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
