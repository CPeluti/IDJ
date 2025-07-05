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
                                                                                  extraProjectiles(0),
	tilemap(tilemap)

{
    this->associated.subject.addObserver(this);

    std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, sprite, 4, 4);
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


    animator->AddAnimation("idle", new Animation(0, 0, 0.1));
    animator->AddAnimation("up", new Animation(0, 2, 0.1));
    animator->AddAnimation("down", new Animation(6, 8, 0.1));
    animator->AddAnimation("right", new Animation(3, 5, 0.1));
    animator->AddAnimation("left", new Animation(9, 11, 0.1));
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
    std::vector<std::string> layers, interactionLayers;
    layers.push_back("layer0");
    interactionLayers.push_back("interaction0, phys0");
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
void Character::Update(float dt)
{
    Vec2 speed = {0, 0};
    this->associated.SetSpeed({0, 0});
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
            speed = {0, 0};
            Command c = taskQueue.front();
            switch (c.type)
            {
            case c.MOVE:
            {
                speed = c.pos.normalized() * m_movementSpeed;
                if(speed.y > 0){
                    animator->SetAnimation("up");
                } else if(speed.y < 0) {
                    animator->SetAnimation("down");
                } else if (speed.x > 0){
                    animator->SetAnimation("right");
                } else if (speed.x < 0){
                    animator->SetAnimation("left");
                } else {
                    animator->SetAnimation("idle");
                }
            }
            break;

            case c.SHOOT:
            {
                if (auto shared = tilemap.lock()) {
                    auto res = associated.CastRaycast(this->associated.box.center(), c.pos, 100, 1);
                    if (res.intersects) {
                        //LOG_INFO("Raycast hit something at x: {} y:{}, not shooting.", res.intersectionPoint.x, res.intersectionPoint.y);
						std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
						std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(*go, "resources/img/fire_placeholder.png", 1, 1);
						go->AddComponent(sr);
                        go->box.Move(res.intersectionPoint * 16);
						Game::GetInstance().GetCurrentState()->AddObject(go);
                    }
                    else {
                        //LOG_INFO("Raycast missed.");

                    }
                }
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
			spell->AddEffect(std::dynamic_pointer_cast<Effect<Spell<Projectile>>>(std::make_shared<MoreProjectileEffect>(10)));
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