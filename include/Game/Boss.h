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

class Boss : public Component, public Observer, public Entity, public std::enable_shared_from_this<Boss> {
public:
    Boss(GameObject& associated, std::string sprite, int frameCountW, int frameCountH) : Component(associated),
        Entity(140),
        hp(500),
        damage(300),
        deathTimer(1),
        m_attackTimer(1),
		frameCountW(frameCountW),
		frameCountH(frameCountH),
		m_movementSpeed(50),
        deathSoundTimer(2),
        roarsTimer(3),
        hitsTimer(0.6f),
        attackTimer(0.6f),
        landTimer(2.5f),
        damageTimer(0.4f)
    {

		//Basic initilization
		this->type = Entity::EnemyType::Enemy;
		this->m_associated = &associated;
        this->associated.subject.addObserver(this);


        //

        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, sprite, frameCountW, frameCountH);
        std::shared_ptr<HealthSystem> hs = std::make_shared<HealthSystem>(associated, hp, true);
        std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);

        Boss::bossCounter++;
        
        Vec2 colliderSize = associated.box.GetSize()*0.6;
        Vec2 colliderCenter = associated.box.GetSize()/2;
		Vec2 colliderOffset = (associated.box.GetSize()-colliderSize) / 2;
        std::shared_ptr<Collider> hitboxCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "enemy", new OnCollisionEvent(associated), colliderSize, Vec2{ 1, 1 }, colliderOffset, true);
        std::shared_ptr<Collider> hurtboxCollider = std::make_shared<Collider>(associated, std::vector<std::string>{"layer0"}, "enemy", new OnCollisionEvent(associated), colliderSize, Vec2{ 1, 1 }, colliderOffset, true);
        m_attackCollider = hitboxCollider;
		m_hurtboxCollider = hurtboxCollider;
        
        associated.AddComponent(sr);
        associated.AddComponent(animator);
        associated.AddComponent(hs);
        associated.AddComponent(hitboxCollider);
        associated.AddComponent(hurtboxCollider);

        animator->AddAnimation("idle", new Animation(0, 0, 0));
        animator->AddAnimation("death", new Animation(0, 0, 0));

        animator->AddAnimation("r_walking", new Animation(0, 3, 0.3f));
        animator->AddAnimation("l_walking", new Animation(4, 7, 0.3f));
        animator->AddAnimation("l_attack", new Animation(8, 12, 0.3f));
        animator->AddAnimation("r_attack", new Animation(13, 17, 0.3f));
        animator->AddAnimation("r_jump", new Animation(18, 28, 0.3f));
        animator->AddAnimation("l_jump", new Animation(29, 39, 0.3f));

        animator->SetAnimation("idle");

        flip = false;

    }
    ~Boss() {
        Boss::bossCounter--;
	}
    class Command
    {
    public:
        enum CommandType
        {
            MOVE,
            ATTACK,
            JUMP
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
        FREEZE,
		JUMPING,
        DAMAGE
    };

    void Start();
    void Update(float dt);
    void Render();
    void OnEvent(Event& evt);
    void SetAnimation(Vec2 direction, Boss::Command::CommandType type);

    inline void Issue(Command task) { taskQueue.push(task); }
    inline Vec2 GetPos() const { return associated.box.center(); }
    inline int GetHP() const { return hp; }
    inline void SetFlip(bool value) { flip = value; }
    inline void SetState(StateType value) { state = value; }
    inline bool Is(std::string type) { return type == "Boss"; }
    
    static int bossCounter;

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

    int m_attackCounter = 0;

    std::weak_ptr<GameObject> m_healthSystem;

    Vec2 JumpStart;
    Vec2 JumpTarget;
    float jumpTime = 0.0f;
    float jumpDuration = 3.0f; // duração total do pulo
	float maxJumpHeight = 100.0f; // altura máxima do pulo

    Timer roarsTimer;
    Timer deathSoundTimer;
    Timer hitsTimer;
    Timer attackTimer;
    Timer landTimer;
    Timer damageTimer;

    int counterRoarsSound = 0;
    int counterHitsSound = 0;
    int counterAttackSound = 0;
    int counterDeathSound = 0;
    int counterLandSound = 0;

    std::vector<Sound*> roarsSound;
    std::vector<Sound*> hitsSound;
    std::vector<Sound*> attackSound;
    std::vector<Sound*> deathSound;
    std::vector<Sound*> landSound;

};