#include <memory>
#include "Core/Game.h"
#include "Core/Component.h"
#include "Core/Camera.h"
#include "Core/SpriteRenderer.h"
#include "Core/Animator.h"
#include "Core/Collider.h"
#include "Core/Log.h"
#include "Core/Text.h"

#include "Game/Character.h"
#include "Game/Bullet.h"
#include "Game/Lifebar.h"
#include "Game/Zombie.h"
#include "Game/Gun.h"
#include "Game/PlayerController.h"
#include "Game/HealthSystem.h"
#include "Game/TypingSystem.h"
#include "Game/Spell.h"

#include "Game/Effect.h"
#include "Game/SpellEffects.h"

void update_color_shader(float r, float g, float b, float a, int color_loc)
{
    float fcolor[4] = {r, g, b, a};
    GPU_SetUniformfv(color_loc, 4, 1, fcolor);
}

std::weak_ptr<Character> Character::player;
int Character::npcCounter = 0;
Character::Character(GameObject &associated, std::string sprite, std::weak_ptr<TileMap> tilemap, bool isPlayer) : Component(associated),
                                                                                  gun(),
                                                                                  taskQueue(),
                                                                                  Entity(140),
                                                                                  hp(500),
                                                                                  isDead(false),
                                                                                  deathTimer(5),
	m_dashTimer(0.4f),
	tilemap(tilemap)

{
    this->associated.subject.addObserver(this);

    std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, sprite, 8, 8);
    std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);

    std::shared_ptr<HealthSystem> hs = std::make_shared<HealthSystem>(associated, hp);

    if (!isPlayer)
    {
        Character::npcCounter++;
    }
    else
    {
        // Shader *shader = sr->GetShader();
        // shader->Load("resources/shaders/common.vert", "resources/shaders/teste.frag");
        // int color_loc = shader->GetLocation("myColor");
        // float t = SDL_GetTicks() / 1000.0f;
        // update_color_shader((1 + sin(t)) / 2, (1 + sin(t + 1)) / 2, (1 + sin(t + 2)) / 2, 1.0f, color_loc);
        std::shared_ptr<PlayerController> playerController = std::make_shared<PlayerController>(associated);
        associated.AddComponent(playerController);
    }

    associated.AddComponent(sr);
    associated.AddComponent(animator);
    associated.AddComponent(hs);


    float dashDuration = m_dashTimer.GetAmount();
    
    animator->AddAnimation("idle", new Animation(0, 0, 0.1));

    animator->AddAnimation("down", new Animation(0, 2, 0.1));
    animator->AddAnimation("downright", new Animation(7, 9, 0.1));
    animator->AddAnimation("right", new Animation(14, 17, 0.1));
    animator->AddAnimation("upright", new Animation(22, 24, 0.1));
    animator->AddAnimation("up", new Animation(29, 31, 0.1));
    animator->AddAnimation("upleft", new Animation(36, 38, 0.1));
    animator->AddAnimation("left", new Animation(43, 46, 0.1));
    animator->AddAnimation("downleft", new Animation(51, 53, 0.1));
    animator->AddAnimation("lookup", new Animation(58, 60, 0.1));

    animator->AddAnimation("dash_down", new Animation(3, 6, dashDuration));
    animator->AddAnimation("dash_downright", new Animation(10, 13, dashDuration));
    animator->AddAnimation("dash_right", new Animation(18, 21, dashDuration));
    animator->AddAnimation("dash_upright", new Animation(25, 28, dashDuration));
    animator->AddAnimation("dash_up", new Animation(32, 35, dashDuration));
    animator->AddAnimation("dash_upleft", new Animation(39, 42, dashDuration));
    animator->AddAnimation("dash_left", new Animation(47, 50, dashDuration));
    animator->AddAnimation("dash_downleft", new Animation(54, 57, dashDuration));
    animator->AddAnimation("dash_lookup", new Animation(61, 64, dashDuration));

    animator->SetAnimation("idle");
    flip = false;
}

Character::~Character()
{
    if (auto character = Character::player.lock())
    {
        if (character.get() != this)
        {
            Character::npcCounter--;
        }
    }
    else
    {
        Character::player.reset();
    }
}

void Character::Start()
{
    Vec2 colliderSize = associated.box.GetSize();
    Vec2 colliderOffset = (colliderSize - associated.box.GetSize());
    std::shared_ptr<Collider> interactionEffectCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"phys0"}, "entity", new OnInteractionEvent(associated, InteractionType::Effect), colliderSize, Vec2{1, 1}, colliderOffset);
    associated.AddComponent(interactionEffectCollider);
    if (shared_from_this() == Character::player.lock())
    {
        std::shared_ptr<GameObject> textObject = std::make_shared<GameObject>();
        std::shared_ptr<Text> textComponent = std::make_shared<Text>(*textObject, "resources/font/neodgm.ttf", 30, Text::SOLID, " ", SDL_Color{255, 255, 255}, 0);
        textObject->AddComponent(textComponent);
        // textObject->box.SetPos(Game::GetInstance().GetWindowSize() / 2 - this->associated.box.GetSize());
        if (auto s = Game::GetInstance().GetCurrentState())
            s->AddObject(textObject);

        TypingSystem &ts = TypingSystem::GetInstance();
        ts.SetTextComponent(textComponent);
    }
}
bool Character::OnDamageTaken(OnDamageTakenEvent &evt)
{
    if (auto animator = std::dynamic_pointer_cast<Animator>(associated.GetComponent("Animator").lock()))
    {
        if (auto hs = std::dynamic_pointer_cast<HealthSystem>(associated.GetComponent("HealthSystem").lock()))
        {
            if (hs->GetHp() <= 0 && !isDead)
            {
                if (auto g = this->gun.lock())
                {
                    g->RequestDelete();
                }
                isDead = true;
                deathTimer.Restart();
                if (Character::player.lock() == shared_from_this())
                {
                    Camera::Unfollow();
                }
            }
            return true;
        }
    }
    return true;
}

void Character::SetAnimation(Vec2 direction)
{
    if (auto animator = std::dynamic_pointer_cast<Animator>(this->associated.GetComponent("Animator").lock()))
    {
        if (direction.x == 0 && direction.y > 0) {
            animator->SetAnimation("down");
        }
        else if (direction.x > 0 && direction.y > 0) {
            animator->SetAnimation("downright");
        }
        else if (direction.x > 0 && direction.y == 0) {
            animator->SetAnimation("right");
        }
        else if (direction.x > 0 && direction.y < 0) {
            animator->SetAnimation("upright");
        }
        else if (direction.x == 0 && direction.y < 0) {
            animator->SetAnimation("up");
        }
        else if (direction.x < 0 && direction.y < 0) {
            animator->SetAnimation("upleft");
        }
        else if (direction.x < 0 && direction.y == 0) {
            animator->SetAnimation("left");
        }
        else if (direction.x < 0 && direction.y > 0) {
            animator->SetAnimation("downleft");
        }
        else {
            animator->SetAnimation("idle");
            LOG_INFO("Character::Update: Moving to {}", direction);
        }
    }
}

void Character::Update(float dt)
{
    std::weak_ptr<Entity> enemy = Entity::GetClosestEnemy(this->associated.box.center(), 200);
    if (auto e = enemy.lock()) {
        if (auto te = targetedEnemy.lock()) {
            if (te != e) {
                targetedEnemy.lock()->SetTargeted(false);
                e->SetTargeted(true);
            }
        }
        targetedEnemy = enemy;
    }

    Vec2 speed = {0, 0};
    this->associated.SetSpeed({0, 0});
    this->UpdateEffects(dt);
	m_dashTimer.Update(dt);
    if (!m_dashTimer.Expired()) {
        LOG_INFO("AQUI");
        speed = (m_lastDirection).normalized() *m_movementSpeed;
        if (speed.x || speed.y)
        {
            Vec2 newSpeed = (speed * dt);
            Vec2 currentPos = associated.box.GetPos();
            if (shared_from_this() == this->player.lock())
            {
                associated.SetSpeed(newSpeed);
            }
        }
        return;
    } else if(m_dashTimer.JustExpired()){
		SetAnimation(m_lastDirection);
        return;
    }
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
            speed = {0, 0};
            Command c = taskQueue.front();
            switch (c.type)
            {
            case c.MOVE:
                {
                    m_lastDirection = c.pos;
                    speed = c.pos.normalized() * m_movementSpeed;
					SetAnimation(m_lastDirection);

                }
                break;

            case c.SHOOT:
            {

                if (auto shared = targetedEnemy.lock()) {
                    CastSpell(SpellType::projectile, SpellElement::fire, {}, shared->GetPosition());
                }
                else {
                    LOG_ERROR("Character::Update: No enemy found to shoot at");
                }
            }
                break;
            case c.DASH:
                {
                    speed = m_lastDirection * m_movementSpeed;
                    LOG_INFO("Character::Update: dashing to {}", m_lastDirection);
                    if (m_lastDirection.x == 0 && m_lastDirection.y > 0) {
                        animator->SetAnimation("dash_down");
                    }
                    else if (m_lastDirection.x > 0 && m_lastDirection.y > 0) {
                        animator->SetAnimation("dash_downright");
                    }
                    else if (m_lastDirection.x > 0 && m_lastDirection.y == 0) {
                        animator->SetAnimation("dash_right");
                    }
                    else if (m_lastDirection.x > 0 && m_lastDirection.y < 0) {
                        animator->SetAnimation("dash_upright");
                    }
                    else if (m_lastDirection.x == 0 && m_lastDirection.y < 0) {
                        animator->SetAnimation("dash_up");
                    }
                    else if (m_lastDirection.x < 0 && m_lastDirection.y < 0) {
                        animator->SetAnimation("dash_upleft");
                    }
                    else if (m_lastDirection.x < 0 && m_lastDirection.y == 0) {
                        animator->SetAnimation("dash_left");
                    }
                    else if (m_lastDirection.x < 0 && m_lastDirection.y > 0) {
                        animator->SetAnimation("dash_downleft");
                    }
                    else {
                        animator->SetAnimation("dash_down");
                    }
					m_dashTimer.Restart();
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
void Character::Render() {}

Character::Command::Command(CommandType type, Vec2 pos) : type(type), pos(pos) {}

void Character::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<OnCollisionEvent>(BIND_EVENT_FN(Character::OnCollision));
    dispatcher.Dispatch<OnDamageTakenEvent>(BIND_EVENT_FN(Character::OnDamageTaken));
    dispatcher.Dispatch<OnEffectEvent<Entity>>(BIND_EVENT_FN(Character::OnEffect));
}

bool Character::OnCollision(OnCollisionEvent &evt)
{
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

bool Character::OnEffect(OnEffectEvent<Entity> &evt)
{
    std::vector<std::weak_ptr<Effect<Entity>>> effects = evt.GetEffects();
    for (auto &effect : effects)
    {
        this->AddEffect(effect);
    }
    return true;
}

void Character::CastSpell(SpellType type, SpellElement element, std::vector<std::shared_ptr<IEffect>> effects, Vec2 target)
{
    switch (type) {
        case SpellType::projectile:
        {
            std::shared_ptr<ProjectileSpell> spell = std::make_shared<ProjectileSpell>(this->associated.box.center(), target);
			//spell->AddEffect(std::dynamic_pointer_cast<Effect<Spell<Projectile>>>(std::make_shared<MoreProjectileEffect>(10)));
            spell->CastSpell();
        }
        
            break;
        case SpellType::area:
        {
            //std::shared_ptr<GameObject> spellObj = std::make_shared<GameObject>();
            //std::shared_ptr<FireAreaSpell> spell = std::make_shared<FireAreaSpell>(*spellObj, this->associated.box.center());
            //spellObj->AddComponent(spell);
            //if (auto s = Game::GetInstance().GetCurrentState())
            //    s->AddObject(spellObj);
            break;
        }
        default:
			break;
    }
}