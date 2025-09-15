#include "Game/Enemy.h"
#include <cmath>

void Enemy::Start() {

    //ParticleSystem initialization
    {
        m_Particle.SizeBegin = 1.0f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0;
        m_Particle.LifeTime = 0.5f;
        m_Particle.Velocity = { 0.0f, 0.0f };
        m_Particle.VelocityVariation = { 4.0f, 4.0f };
        m_Particle.Position = { 0.0f, 0.0f };
        m_Particle.VelocityFunction = { [](float x) {return x == 1 ? 1 : 1 - pow(2, -10 * x); },[](float x) {return x == 1 ? 1 : 1 - pow(2, -10 * x); } };
    }
    std::shared_ptr<GameObject> particles = std::make_shared<GameObject>();
    std::shared_ptr<ParticleSystem> ps = std::make_shared<ParticleSystem>(*particles, m_Particle);
    particles->AddComponent(ps);
    particles->z = 1;
    ps->SetAmount(50);
    ps->SetExplosiveness(1);
    ps->SetOneshot(true);
    particlesSystem = Game::GetInstance().GetCurrentState()->AddObject(particles);

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

    this->m_enemies.push_back(shared_from_this());
}

void Enemy::SetAnimation(Vec2 direction, Enemy::Command::CommandType type)
{
    if (auto animator = std::dynamic_pointer_cast<Animator>(this->associated.GetComponent("Animator").lock()))
    {
        if (type == Enemy::Command::MOVE) {
            if (direction.x == 0 && direction.y > 0)
            {
                animator->SetAnimation("down_walking");
            }
            else if (direction.x > 0 && direction.y > 0)
            {
                //animator->SetAnimation("downright");
                animator->SetAnimation("r_walking");
            }
            else if (direction.x > 0 && direction.y == 0)
            {
                animator->SetAnimation("r_walking");
            }
            else if (direction.x > 0 && direction.y < 0)
            {
                animator->SetAnimation("r_up_walking");
            }
            else if (direction.x == 0 && direction.y < 0)
            {
                animator->SetAnimation("up_walking");
            }
            else if (direction.x < 0 && direction.y < 0)
            {
                animator->SetAnimation("l_up_walking");
            }
            else if (direction.x < 0 && direction.y == 0)
            {
                animator->SetAnimation("l_walking");
            }
            else if (direction.x < 0 && direction.y > 0)
            {
                //animator->SetAnimation("downleft");
                animator->SetAnimation("l_walking");
            }
        }
        else if (type == Enemy::Command::ATTACK) {
            if (direction.x == 0 && direction.y > 0)
            {
                animator->SetAnimation("down_attack");
            }
            else if (direction.x > 0 && direction.y > 0)
            {
                //animator->SetAnimation("downright");
                animator->SetAnimation("r_down_attack");
            }
            else if (direction.x > 0 && direction.y == 0)
            {
                animator->SetAnimation("r_walking");
            }
            else if (direction.x > 0 && direction.y < 0)
            {
                animator->SetAnimation("r_up_attack");
            }
            else if (direction.x == 0 && direction.y < 0)
            {
                animator->SetAnimation("up_attack");
            }
            else if (direction.x < 0 && direction.y < 0)
            {
                animator->SetAnimation("l_up_attack");
            }
            else if (direction.x < 0 && direction.y == 0)
            {
                animator->SetAnimation("l_walking");
            }
            else if (direction.x < 0 && direction.y > 0)
            {
                //animator->SetAnimation("downleft");
                animator->SetAnimation("l_down_attack");
            }
        }
        else
        {
            animator->SetAnimation("idle");
        }
    }
}

void Enemy::Update(float dt)
{
    Vec2 speed = { 0, 0 };
    this->associated.SetSpeed({ 0, 0 });
    this->UpdateEffects(dt);
	this->m_attackTimer.Update(dt);
    if (auto sr = std::dynamic_pointer_cast<SpriteRenderer>(this->associated.GetComponent("SpriteRenderer").lock()))
    {
        if (state != StateType::DYING) {
            if (auto shader = sr->GetShader().lock())
            {
                if (this->m_freeze)
                {
                    shader->Load("resources/shaders/common.vert", "resources/shaders/color_overlay.frag");
                    int color_loc = shader->GetLocation("colorOverlay");
                    float t = SDL_GetTicks() / 1000.0f;
                    update_color_shader(0.0f,.5f,.5f,1.0f, color_loc);
                    state = StateType::FREEZE;
                }
                else if (this->m_targeted)
                {
                    shader->Load("resources/shaders/common.vert", "resources/shaders/outline.frag");
                    int color_loc = shader->GetLocation("myColor");
                    float t = SDL_GetTicks() / 1000.0f;
                    //update_color_shader(255,255,255,255,0);
                }
                else
                {
                    shader->Reset();
                }
            }
        }
    }
    if (auto animator = std::dynamic_pointer_cast<Animator>(this->associated.GetComponent("Animator").lock()))
    {

        if (taskQueue.size() == 0 && animator)
        {
            //animator->SetAnimation("idle");
        }
        while (taskQueue.size() > 0)
        {
            speed = { 0, 0 };
			//m_movementSpeed = 10;
            Command c = taskQueue.front();
            switch (c.type)
            {
            case c.MOVE:
            {
                //if (m_attackTimer.Expired()) {
                m_lastDirection = c.pos;
                speed = c.pos.normalized() * m_movementSpeed;
                SetAnimation(m_lastDirection, c.type);
                //}
            }
            break;

            case c.ATTACK:
            {
				/*SetState(StateType::ATTACKING);
                m_attackTimer.Restart();
                if (auto c = attackCollider.lock()) {
                    c->SetDisable(false);
                }*/
                speed = m_lastDirection * m_movementSpeed * 2;
				m_movementSpeed = 0;
                SetAnimation(m_lastDirection, c.type);

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
				this->associated.SetSpeed(newSpeed);
            }
        }
		Raycast res;
        if (auto character = Character::player.lock()) {
            Vec2 playerPos = character->GetPos();
            res = associated.CastRaycast(this->associated.box.center(), playerPos, 100, 1);
            if (!res.intersects) {
                m_lastSeenPlayerPosition = playerPos;
            }
        }
        switch (state) {
            case StateType::IDLE: {
                /*if (auto character = Character::player.lock()) {
                    Vec2 playerPos = character->GetPos();
                    Vec2 distance = (playerPos - associated.box.center()).normalized();
                    if (auto enemy = std::dynamic_pointer_cast<Enemy>(this->associated.GetComponent("Enemy").lock())) {
                        enemy->Issue(Enemy::Command(Enemy::Command::MOVE, distance));
                    }
                }*/
                if (res.intersects) {
                    animator->SetAnimation("idle");
                }
                else {
                    SetState(StateType::MOVING);
                }
            }
            break;
            case StateType::ATTACKING: {

                if (auto c = m_attackCollider.lock()) {
                    c->SetDisable(false);
                }
                if (auto character = Character::player.lock()) {
                    if (!res.intersects) {
                        Vec2 playerPos = character->GetPos();
                        this->Issue(Enemy::Command(Enemy::Command::ATTACK, playerPos));
                    }
                }
                if (m_attackTimer.Expired()) {
					m_attackTimer.Restart();
                    attacked = false;
                }
                if (m_attackTimer.JustExpired())
                {
                    if (auto c = m_attackCollider.lock()) {
                        c->SetDisable(true);
                    }
                    SetState(StateType::IDLE);
                }
            }
            break;
            case StateType::MOVING: {
                if (auto character = Character::player.lock()) {
                    Vec2 playerPos = character->GetPos();
                    Vec2 distance = (m_lastSeenPlayerPosition - associated.box.center()).normalized();
                    if (res.intersects && Vec2::Distance(m_lastSeenPlayerPosition, this->associated.box.center()) < 50) {
                        SetState(StateType::IDLE);
                    }
                    else if (auto enemy = std::dynamic_pointer_cast<Enemy>(this->associated.GetComponent("Enemy").lock())) {
                        if (Vec2::Distance(playerPos, this->associated.box.center()) < 30) {
                            SetState(StateType::ATTACKING);
                        }
                        else {
                            enemy->Issue(Enemy::Command(Enemy::Command::MOVE, distance));
                        }
                    }
                }
            }
            break;
            case StateType::DYING: {
                animator->SetAnimation("death");
                if (auto c = m_attackCollider.lock())
                    c->SetDisable(true);
                if (auto c = m_hurtboxCollider.lock())
                    c->SetDisable(true);
                deathTimer.Update(dt);
                if (deathTimer.Expired())
                {
                    if (auto particles = particlesSystem.lock()) {
                        if (auto ps = std::dynamic_pointer_cast<ParticleSystem>(particles->GetComponent("ParticleSystem").lock())) {
                            ps->Play();
                            if (!ps->emmiting) {
								associated.RequestDelete();
                            }
                        }
                    }
                }
                return;
			}
            case StateType::FREEZE: {
                if(m_freeze)
                    animator->SetAnimation("death");
                else
					state = StateType::IDLE;
                break;
            }
        }
    }
    if (auto particles = particlesSystem.lock()) {
        particles->box.Move(this->associated.box.center());
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
            if (hs->GetHp() <= 0)
            {
                if (auto sr = std::dynamic_pointer_cast<SpriteRenderer>(associated.GetComponent("SpriteRenderer").lock()))
                {
                    if(auto shader = sr->GetShader().lock())
                    {
                        shader->Load("resources/shaders/common.vert", "resources/shaders/hit.frag");
                        int color_loc = shader->GetLocation("myColor");
                        float t = SDL_GetTicks() / 1000.0f;
					}
                }
                deathTimer.Restart();
                state = StateType::DYING;
                deathSound.Play();
            }
            return true;
        }
    }
    return true;
}

bool Enemy::OnCollision(OnCollisionEvent& evt) {
    GameObject& go = evt.GetGameObject();
    OnDamageTakenEvent e = OnDamageTakenEvent(this->associated, this->damage);

    if (go.GetComponent("HealthSystem").lock() && !attacked) {
        go.subject.notify(e);
		attacked = true;
    }

    return true;
}

bool Enemy::OnInteraction(OnInteractionEvent& evt)
{
    GameObject& target = evt.GetGameObject();
    switch (evt.GetInteractionType())
    {
    case InteractionType::Effect:
    {
        std::vector<Effect<Entity>*> effects;
        for (auto& effect : this->m_Effects)
        {
            Effect<Entity>* newWeakEffect = effect;
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

bool Enemy::OnEffect(OnEffectEvent<Entity>& evt)
{
    std::vector<Effect<Entity>*> effects = evt.GetEffects();
    for (auto& effect : effects)
    {
        this->AddEffect(effect);
    }
    return true;
}

int Enemy::enemyCounter = 0;