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
#include "Game/ManaSystem.h"
#include "Game/TypingSystem.h"
#include "Game/Spell.h"

#include "Game/Effect.h"
#include "Game/SpellEffects.h"

std::weak_ptr<Character> Character::player;
int Character::npcCounter = 0;
Character::Character(GameObject &associated, std::string sprite, std::weak_ptr<TileMap> tilemap, bool isPlayer) : Component(associated),
                                                                                                                  gun(),
                                                                                                                  taskQueue(),
                                                                                                                  Entity(140),
                                                                                                                  hp(500),
                                                                                                                  mana(100),
                                                                                                                  isDead(false),
                                                                                                                  deathTimer(5),
                                                                                                                  m_dashTimer(0.4f),
	                                                                                                              m_idleTimer(1.2f),
                                                                                                                  tilemap(tilemap)

{
    this->associated.subject.addObserver(this);

    std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, sprite, 86, 1);
    std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);

    std::shared_ptr<HealthSystem> hs = std::make_shared<HealthSystem>(associated, hp);
    std::shared_ptr<ManaSystem> ms = std::make_shared<ManaSystem>(associated, mana);

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
    associated.AddComponent(ms);

    float dashDuration = m_dashTimer.GetAmount();




    animator->AddAnimation("idle_blink", new Animation(6, 6, 0.5));


    animator->AddAnimation("idle_up", new Animation(0, 2, 0.5));
    animator->AddAnimation("idle_down", new Animation(3, 5, 0.5));
    animator->AddAnimation("idle_right", new Animation(7, 9, 0.5));
    animator->AddAnimation("idle_left", new Animation(10, 12, 0.5));
    animator->AddAnimation("idle_downright", new Animation(13, 15, 0.5));
    animator->AddAnimation("idle_downleft", new Animation(16, 18, 0.5));
    animator->AddAnimation("idle_upright", new Animation(19, 21, 0.5));
    animator->AddAnimation("idle_upleft", new Animation(22, 24, 0.5));

    animator->AddAnimation("walk_up", new Animation(25, 27, 0.1));
    animator->AddAnimation("walk_down", new Animation(28, 30, 0.1));
    animator->AddAnimation("walk_right", new Animation(31, 34, 0.1));
    animator->AddAnimation("walk_left", new Animation(35, 38, 0.1));
    animator->AddAnimation("walk_downright", new Animation(39, 41, 0.1));
    animator->AddAnimation("walk_downleft", new Animation(42, 44, 0.1));
    animator->AddAnimation("walk_upright", new Animation(45, 47, 0.1));
    animator->AddAnimation("walk_upleft", new Animation(48, 50, 0.1));

    animator->AddAnimation("dash_up", new Animation(51, 54, dashDuration/4));
    animator->AddAnimation("dash_down", new Animation(55, 58, dashDuration/4));
    animator->AddAnimation("dash_right", new Animation(59, 62, dashDuration/4));
    animator->AddAnimation("dash_left", new Animation(63, 66, dashDuration/4));
    animator->AddAnimation("dash_downright", new Animation(67, 70, dashDuration/4));
    animator->AddAnimation("dash_downleft", new Animation(71, 74, dashDuration/4));
    animator->AddAnimation("dash_upright", new Animation(75, 78, dashDuration/4));
    animator->AddAnimation("dash_upleft", new Animation(79, 82, dashDuration/4));

    animator->AddAnimation("lookup", new Animation(83, 85, 0.1));

    animator->SetAnimation("idle_down");
    flip = false;

    SpellAssets fireProjectile = {
        "resources/img/fogo_projetil.png",
        "resources/audio/FireBall_Cast.wav",
        "resources/audio/FireBall_Hit_Strong",
        new Animation(0, 7, 0.1f),
        8,
        1
    };
	std::tuple<SpellElement, SpellType, SpellAssets> fireProjectileTuple = std::make_tuple(SpellElement::fire, SpellType::projectile, fireProjectile);
	m_spellAssets.push_back(fireProjectileTuple);

    SpellAssets fireArea = {
        "resources/img/fogo_area.png",
        "resources/audio/FireArea_Explode.wav",
        "resources/audio/FireBall_Hit_Strong.wav",
        new Animation(0, 8, 0.1f),
        9,
        1
    };
    std::tuple<SpellElement, SpellType, SpellAssets> fireAreTuple = std::make_tuple(SpellElement::fire, SpellType::area, fireArea);
    m_spellAssets.push_back(fireAreTuple);
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
    m_idleTimer.Restart();
    Vec2 colliderSize = associated.box.GetSize();
    Vec2 colliderOffset = (colliderSize - associated.box.GetSize());
    std::shared_ptr<Collider> physicsCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"phys0"}, "entity", new OnInteractionEvent(associated, InteractionType::Effect), colliderSize, Vec2{ 1, 1 }, colliderOffset);
    std::shared_ptr<Collider> hurtboxCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "player", new OnCollisionEvent(associated), colliderSize, Vec2{1, 1}, colliderOffset);
    associated.AddComponent(physicsCollider);
    associated.AddComponent(hurtboxCollider);
    if (shared_from_this() == Character::player.lock())
    {
        std::shared_ptr<GameObject> textObject = std::make_shared<GameObject>();
        std::shared_ptr<Text> textComponent = std::make_shared<Text>(*textObject, "resources/font/neodgm.ttf", 30/Camera::zoom, Text::SOLID, " ", SDL_Color{255, 255, 255}, 0);
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
    if (&(this->associated) == &(evt.GetDealer())) {
        return true;
    }
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

void Character::SetAnimation(Vec2 direction, std::string prefix)
{
    std::string newDirection = "";
    if (auto animator = std::dynamic_pointer_cast<Animator>(this->associated.GetComponent("Animator").lock()))
    {
        if (direction.x == 0 && direction.y > 0)
        {
            newDirection = "down";
        }
        else if (direction.x > 0 && direction.y > 0)
        {
            newDirection = "downright";
        }
        else if (direction.x > 0 && direction.y == 0)
        {
            newDirection = "right";
        }
        else if (direction.x > 0 && direction.y < 0)
        {
            newDirection = "upright";
        }
        else if (direction.x == 0 && direction.y < 0)
        {
            newDirection = "up";
        }
        else if (direction.x < 0 && direction.y < 0)
        {
            newDirection = "upleft";
        }
        else if (direction.x < 0 && direction.y == 0)
        {
            newDirection = "left";
        }
        else if (direction.x < 0 && direction.y > 0)
        {
            newDirection = "downleft";
        }
        m_lastDirection = { direction, newDirection };
  //      if(m_idleTimer.Expired() && prefix == "idle")
  //      {
  //          if (m_lastDirection.name == "down")
  //          {
  //              animator->SetAnimation(fmt::format("idle_blink"));
  //              m_idleTimer.Restart();
  //          }
  //          else
  //          {
  //              animator->SetAnimation(fmt::format("{}_{}", prefix, newDirection));
  //          }
  //      }
		//else
        animator->SetAnimation(fmt::format("{}_{}", prefix, newDirection));
    }
}

void Character::Update(float dt)
{
    TypingSystem& ts = TypingSystem::GetInstance();

    if (auto ms = std::dynamic_pointer_cast<ManaSystem>(this->associated.GetComponent("ManaSystem").lock())) {
		ms->m_manaRegenTimer.Update(dt);
        if (ms->m_manaRegenTimer.Expired() && !(ms->isManaFull())) {
			ms->IncreaseMana();
            ms->m_manaRegenTimer.Restart();
        }
    }

    std::weak_ptr<Entity> enemy = Entity::GetClosestEnemy(this->associated.box.center(), 200);
    if (auto e = enemy.lock())
    {
        auto raycast = this->associated.CastRaycast(e->GetPosition(), this->associated.box.center(), 5000, 1);

        if (!raycast.intersects) {
            if (auto te = targetedEnemy.lock())
            {
                if (te != e)
                {
                    targetedEnemy.lock()->SetTargeted(false);
                }
            }
            targetedEnemy = enemy;
            e->SetTargeted(true);
        }
    }

    Vec2 speed = {0, 0};
    this->associated.SetSpeed({0, 0});
    this->UpdateEffects(dt);
    m_dashTimer.Update(dt);
	m_idleTimer.Update(dt);
    if (!m_dashTimer.Expired())
    {
        speed = m_lastDirection.coordinates.normalized() * m_movementSpeed;
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
    }
    else if (m_dashTimer.JustExpired())
    {
        SetAnimation(m_lastDirection.coordinates, "walk");
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
            SetAnimation(m_lastDirection.coordinates,"idle");
        }
        while (taskQueue.size() > 0)
        {
            speed = {0, 0};
            Command c = taskQueue.front();
            switch (c.type)
            {
            case c.MOVE:
            {
                m_lastDirection.coordinates = c.pos;
                speed = c.pos.normalized() * m_movementSpeed;
                SetAnimation(m_lastDirection.coordinates,"walk");
            }
            break;

            case c.SHOOT:
            {
                std::vector<std::string> spellText = ts.GetSubmittedText();

                if (auto shared = targetedEnemy.lock())
                {
                    CastSpell({}, shared->GetPosition(), spellText);
                }
                else
                {
                    LOG_ERROR("Character::Update: No enemy found to shoot at");
                }
                
            }
            break;
            case c.DASH:
            {
                Vec2 coordinates = m_lastDirection.coordinates;
                speed = m_lastDirection.coordinates * m_movementSpeed;
                LOG_INFO("Character::Update: dashing to {}", m_lastDirection);
				SetAnimation(coordinates, "dash");
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
    std::vector<Effect<Entity>*> effects = evt.GetEffects();
    for (auto &effect : effects)
    {
        this->AddEffect(effect);
    }
    return true;
}

void Character::CastSpell(std::vector<std::shared_ptr<IEffect>> effects, Vec2 target, std::vector<std::string> spellText)
{
    auto itFoundElement = spellElements.end();
    auto itFoundType = spellType.end();
    for (std::string s : spellText) {
        itFoundElement = spellElements.find(s);
        if (itFoundElement != spellElements.end()) {
            break;
		}
    }
    for (std::string s : spellText) {
        itFoundType = spellType.find(s);
        if (itFoundType != spellType.end()) {
            break;
		}
    }
    if (itFoundElement != spellElements.end() && itFoundType != spellType.end()) {
        
        SpellAssets assets;
        for (std::tuple<SpellElement, SpellType, SpellAssets> spellTuple : m_spellAssets) {
            if (std::get<0>(spellTuple) == itFoundElement->second && std::get<1>(spellTuple) == itFoundType->second)
            {
			    assets = std::get<2>(spellTuple);
                break;
		    }
        }

        switch (itFoundType->second)
        {
        case SpellType::projectile:
        {
            std::shared_ptr<ProjectileSpell> spell = std::make_shared<ProjectileSpell>(this->associated.box.center(), target, assets);
            //spell->AddEffect(std::dynamic_pointer_cast<Effect<Spell<Projectile>>>(std::make_shared<MoreProjectileEffect>(5)));
            spell->CastSpell(&this->associated);
        }

        break;
        case SpellType::area:
        {
            std::shared_ptr<AreaSpell> spell = std::make_shared<AreaSpell>(target, assets);
            spell->CastSpell(&this->associated);
            // std::shared_ptr<GameObject> spellObj = std::make_shared<GameObject>();
            // std::shared_ptr<FireAreaSpell> spell = std::make_shared<FireAreaSpell>(*spellObj, this->associated.box.center());
            // spellObj->AddComponent(spell);
            // if (auto s = Game::GetInstance().GetCurrentState())
            //     s->AddObject(spellObj);
            break;
        }
        default:
            break;
        }
    }
}