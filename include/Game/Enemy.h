#include "Core/GameObject.h"
#include "Core/TileMap.h"
#include "Core/Component.h"
#include "Core/SpriteRenderer.h"
#include "Core/Animator.h"

#include "Game/HealthSystem.h"

#include "Entity.h"

#include <queue>

class Enemy : public Component, public Observer, public Entity {
public:
    Enemy(GameObject& associated, std::string sprite) : Component(associated),
        Entity(140),
        hp(500),
        isDead(false),
        deathTimer(5)
    {
        this->associated.subject.addObserver(this);

        std::shared_ptr<SpriteRenderer> sr = std::make_shared<SpriteRenderer>(associated, sprite, 4, 4);
        std::shared_ptr<HealthSystem> hs = std::make_shared<HealthSystem>(associated, hp);
        std::shared_ptr<Animator> animator = std::make_shared<Animator>(associated);

		Enemy::enemyCounter++;
        
        associated.AddComponent(sr);
        associated.AddComponent(animator);
        associated.AddComponent(hs);

        animator->AddAnimation("walking", new Animation(0, 3, 0.3));
        animator->AddAnimation("r_walking", new Animation(0, 3, 0.3, SDL_FLIP_HORIZONTAL));
        animator->AddAnimation("dead", new Animation(5, 5, 0));
        animator->AddAnimation("hit", new Animation(4, 4, 0));
        animator->AddAnimation("r_hit", new Animation(4, 4, 0, SDL_FLIP_HORIZONTAL));

        /*animator->AddAnimation("idle", new Animation(0, 0, 0.1));
        animator->AddAnimation("up", new Animation(0, 2, 0.1));
        animator->AddAnimation("down", new Animation(6, 8, 0.1));
        animator->AddAnimation("right", new Animation(3, 5, 0.1));
        animator->AddAnimation("left", new Animation(9, 11, 0.1));
        animator->SetAnimation("idle");*/
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
            SHOOT
        };
        Command(CommandType type, Vec2 pos);
        CommandType type;
        Vec2 pos;
    };

    void Start();
    void Update(float dt);
    void Render();
    void OnEvent(Event& evt);

    inline void Issue(Command task) { taskQueue.push(task); }
    inline Vec2 GetPos() const { return associated.box.center(); }
    inline int GetHP() const { return hp; }
    inline void SetFlip(bool value) { flip = value; }
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
    bool isDead;
    Subject subject;
    Timer deathTimer;
    std::queue<Command> taskQueue;
    std::weak_ptr<TileMap> tilemap;
    std::vector<std::shared_ptr<IEffect>> effects;
};