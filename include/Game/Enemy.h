#include "Core/GameObject.h"
#include "Core/TileMap.h"
#include "Core/Component.h"
#include "Core/SpriteRenderer.h"
#include "Core/Animator.h"
#include "Core/ParticleSystem.h"

#include "Game/HealthSystem.h"
#include "Game/Character.h"

#include "Entity.h"

#include <queue>

class Enemy : public Component, public Observer, public Entity, public std::enable_shared_from_this<Enemy> {
public:
    Enemy(GameObject& associated, std::string sprite, int frameCountW, int frameCountH) : Component(associated),
        Entity(140),
        hp(500),
        damage(10),
        deathTimer(1),
        m_attackTimer(1),
		frameCountW(frameCountW),
		frameCountH(frameCountH),
		m_movementSpeed(50)
    {

		//Basic initilization
		this->type = Entity::EnemyType::Enemy;
		this->m_associated = &associated;
        this->associated.subject.addObserver(this);


        //
        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, sprite, frameCountW, frameCountH);
        std::shared_ptr<HealthSystem> hs = std::make_shared<HealthSystem>(associated, hp);
        std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);

		Enemy::enemyCounter++;
        
        Vec2 colliderSize = associated.box.GetSize();
        Vec2 colliderOffset = (colliderSize - associated.box.GetSize());
        std::shared_ptr<Collider> hitboxCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "enemy", new OnCollisionEvent(associated), colliderSize, Vec2{ 1, 1 }, colliderOffset, true);
        std::shared_ptr<Collider> hurtboxCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "enemy", new OnCollisionEvent(associated), colliderSize, Vec2{ 1, 1 }, colliderOffset, true);
        m_attackCollider = hitboxCollider;
		m_hurtboxCollider = hurtboxCollider;
        
        associated.AddComponent(sr);
        associated.AddComponent(animator);
        associated.AddComponent(hs);
        associated.AddComponent(hitboxCollider);
        associated.AddComponent(hurtboxCollider);

        animator->AddAnimation("idle", new Animation(48, 53, 0.5));
        animator->AddAnimation("death", new Animation(48, 48, 0));

        animator->AddAnimation("down_walking", new Animation(40, 43, 0.3));
        animator->AddAnimation("up_walking", new Animation(44, 47, 0.3));
        animator->AddAnimation("r_walking", new Animation(32, 35, 0.3));
        animator->AddAnimation("l_walking", new Animation(32, 35, 0.3, SDL_FLIP_HORIZONTAL));
        animator->AddAnimation("r_up_walking", new Animation(36, 39, 0.3));
        animator->AddAnimation("l_up_walking", new Animation(36, 39, 0.3, SDL_FLIP_HORIZONTAL));

        animator->AddAnimation("r_down_attack", new Animation(8, 13, 0.1));
        animator->AddAnimation("l_down_attack", new Animation(8, 13, 0.1, SDL_FLIP_HORIZONTAL));
        animator->AddAnimation("r_up_attack", new Animation(14, 19, 0.1));
        animator->AddAnimation("l_up_attack", new Animation(14, 19, 0.1, SDL_FLIP_HORIZONTAL));
        animator->AddAnimation("down_attack", new Animation(20, 25, 0.1));
        animator->AddAnimation("up_attack", new Animation(26, 31, 0.1));

        animator->AddAnimation("r_aggro", new Animation(0, 7, 0.09));
        animator->AddAnimation("l_aggro", new Animation(0, 7, 0.09, SDL_FLIP_HORIZONTAL));

        animator->SetAnimation("idle");

        flip = false;

    }
    ~Enemy() {
        Enemy::enemyCounter--;
	}
    class Command
    {
    public:
        enum CommandType
        {
            MOVE,
            ATTACK
        };
        Command(CommandType type, Vec2 pos);
        CommandType type;
        Vec2 pos;
    };
    enum StateType
    {
        IDLE,
        MOVING,
        ATTACKING,
        DYING,
        FREEZE
    };

    void Start();
    void Update(float dt);
    void Render();
    void OnEvent(Event& evt);
    void SetAnimation(Vec2 direction, Enemy::Command::CommandType type);

    inline void Issue(Command task) { taskQueue.push(task); }
    inline Vec2 GetPos() const { return associated.box.center(); }
    inline int GetHP() const { return hp; }
    inline void SetFlip(bool value) { flip = value; }
    inline void SetState(StateType value) { state = value; }
    inline bool Is(std::string type) { return type == "Enemy"; }
    
    static int enemyCounter;

private:
    bool OnCollision(OnCollisionEvent& evt);
    bool OnDamageTaken(OnDamageTakenEvent& evt);
    bool OnInteraction(OnInteractionEvent& evt);
    bool OnEffect(OnEffectEvent<Entity>& evt);

    float hp;
    bool flip;
    Vec2 speed;
    float damage;
    Subject subject;
    Timer deathTimer;
	Timer m_attackTimer;
    int frameCountW;
    int frameCountH;
    Vec2 m_lastDirection;
    Vec2 m_lastSeenPlayerPosition;
	float m_movementSpeed;
    std::weak_ptr<GameObject> exclamation;
    bool playerFound = false;
    /*bool moving = false;*/
    bool attacked = false;
	StateType state = StateType::IDLE;
    Vec2 characterBreadcrumb;
    std::queue<Command> taskQueue;
    std::weak_ptr<TileMap> tilemap;
    std::vector<std::shared_ptr<IEffect>> effects;
    std::weak_ptr<Collider> m_attackCollider;
    std::weak_ptr<Collider> m_hurtboxCollider;

    ParticleData m_Particle;
    std::weak_ptr<GameObject> particlesSystem;

};