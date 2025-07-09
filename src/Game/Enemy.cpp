#include "Game/Enemy.h"

Enemy::Start() {
    std::vector<std::string> layers, interactionLayers;
    layers.push_back("layer0");
    interactionLayers.push_back("interaction0, phys0");
    Vec2 colliderSize = associated.box.GetSize();
    Vec2 colliderOffset = (colliderSize - associated.box.GetSize());
    std::shared_ptr<Collider> interactionEffectCollider = std::make_shared<Collider>(
        associated, 
        std::vector<std::string>{"phys0"}, 
        "entity", 
        new OnInteractionEvent(associated, InteractionType::Effect), 
        colliderSize, 
        Vec2{ 1, 1 }, 
        colliderOffset
    );
    associated.AddComponent(interactionEffectCollider);
}

void Enemy::Update(float dt)
{
    Vec2 speed = { 0, 0 };
    this->associated.SetSpeed({ 0, 0 });
    this->UpdateEffects(dt);
    if (auto animator = std::dynamic_pointer_cast<Animator>(this->associated.GetComponent("Animator").lock()))
    {
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
            animator->SetAnimation("idle");
        }
        while (taskQueue.size() > 0)
        {
            speed = { 0, 0 };
            Command c = taskQueue.front();
            switch (c.type)
            {
            case c.MOVE:
            {
                speed = c.pos.normalized() * m_movementSpeed;
                if (speed.y > 0) {
                    animator->SetAnimation("up");
                }
                else if (speed.y < 0) {
                    animator->SetAnimation("down");
                }
                else if (speed.x > 0) {
                    animator->SetAnimation("right");
                }
                else if (speed.x < 0) {
                    animator->SetAnimation("left");
                }
                else {
                    animator->SetAnimation("idle");
                }
            }
            break;

            case c.SHOOT:
            {
                //if (auto shared = tilemap.lock()) {
                //    auto res = associated.CastRaycast(this->associated.box.center(), c.pos, 100, 1);
                //    if (res.intersects) {
                //        //LOG_INFO("Raycast hit something at x: {} y:{}, not shooting.", res.intersectionPoint.x, res.intersectionPoint.y);
                //        std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
                //        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(*go, "resources/img/fire_placeholder.png", 1, 1);
                //        go->AddComponent(sr);
                //        go->box.Move(res.intersectionPoint * 16);
                //        Game::GetInstance().GetCurrentState()->AddObject(go);
                //    }
                //    else {
                //        //LOG_INFO("Raycast missed.");

                //    }
                //}
            }
            break;
            }
            taskQueue.pop();
            if (speed.x || speed.y)
            {
                Vec2 newSpeed = (speed * dt);
                Vec2 currentPos = associated.box.GetPos();
                if (shared_from_this() == this->player.lock())
                {
                    associated.SetSpeed(newSpeed);
                }
            }
        }
    }
}

void Enemy::Render() {}

Enemy::Command::Command(CommandType type, Vec2 pos) : type(type), pos(pos) {}

void Enemy::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Enemy::OnCollision));
    dispatcher.Dispatch<OnDamageTakenEvent>(BIND_EVENT_FN(Enemy::OnDamageTaken));
    dispatcher.Dispatch<OnEffectEvent<Entity>>(BIND_EVENT_FN(Enemy::OnEffect));
}

bool Enemy::OnDamageTaken(OnDamageTakenEvent& evt)
{
    if (auto animator = std::dynamic_pointer_cast<Animator>(associated.GetComponent("Animator").lock()))
    {
        if (auto hs = std::dynamic_pointer_cast<HealthSystem>(associated.GetComponent("HealthSystem").lock()))
        {
            if (hs->GetHp() <= 0 && !isDead)
            {
                isDead = true;
                deathTimer.Restart();
            }
            return true;
        }
    }
    return true;
}

bool Enemy::OnCollision(OnCollisionEvent& evt) {
    GameObject& go = evt.GetGameObject();
    OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->damage);

    if (go.GetComponent("HealthSystem").lock())
        go.subject.notify(e);

    return true;
}

bool Enemy::OnInteraction(OnInteractionEvent& evt)
{
    GameObject& target = evt.GetGameObject();
    switch (evt.GetInteractionType())
    {
    case InteractionType::Effect:
    {
        std::vector<std::weak_ptr<Effect<Entity>>> effects;
        for (auto& effect : this->m_Effects)
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

bool Character::OnEffect(OnEffectEvent<Entity>& evt)
{
    std::vector<std::weak_ptr<Effect<Entity>>> effects = evt.GetEffects();
    for (auto& effect : effects)
    {
        this->AddEffect(effect);
    }
    return true;
}