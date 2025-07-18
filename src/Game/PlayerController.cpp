#include "Core/InputManager.h"

#include "Game/Character.h"
#include "Game/Dummy.h"
#include "Game/Enemy.h"
#include "Game/Boss.h"
#include "Game/PlayerController.h"
#include "Game/TypingSystem.h"

PlayerController::PlayerController(GameObject &associated) : Component(associated) {}

void PlayerController::Start() {}
void PlayerController::Render() {}

void PlayerController::Update(float dt)
{
    InputManager ip = InputManager::GetInstance();
    TypingSystem &ts = TypingSystem::GetInstance();

    Vec2 direction = {0, 0};

    if (ip.IsKeyDown(A_KEY))
    {
        direction.x -= 1;
    }
    if (ip.IsKeyDown(D_KEY))
    {
        direction.x += 1;
    }
    if (ip.IsKeyDown(W_KEY))
    {
        direction.y -= 1;
    }
    if (ip.IsKeyDown(S_KEY))
    {
        direction.y += 1;
    }
    if (direction.x || direction.y)
    {
        Character::Command c = Character::Command(Character::Command::MOVE, direction);
        if(auto character = std::dynamic_pointer_cast<Character>(this->associated.GetComponent("Character").lock())){
            character->Issue(c);
        }
    }
    if (ip.MousePress(LEFT_MOUSE_BUTTON))
    {
        Vec2 target;
        target.x = ip.GetMouseX();
        target.y = ip.GetMouseY();
        Character::Command c = Character::Command(Character::Command::SHOOT, target);
        if(auto character = std::dynamic_pointer_cast<Character>(this->associated.GetComponent("Character").lock())){
            character->Issue(c);
        }
    }
    if (ip.MousePress(RIGHT_MOUSE_BUTTON))
    {
        /*std::shared_ptr<GameObject> dummy = std::make_shared<GameObject>();
        std::shared_ptr<Dummy> dummyComponent = std::make_shared<Dummy>(*dummy);
		dummy->box.Move({ ip.GetMouseX(), ip.GetMouseY() });
        dummy->AddComponent(dummyComponent);
        if(auto s = Game::GetInstance().GetCurrentState())
        {
            s->AddObject(dummy);
		}*/

        std::shared_ptr<GameObject> enemy = std::make_shared<GameObject>();
        std::shared_ptr<Boss> enemyComponent = std::make_shared<Boss>(*enemy, "resources/img/Sprite-Shoggoth-All.png", 40, 1);
        enemy->box.Move({ ip.GetMouseX(), ip.GetMouseY() });
        enemy->AddComponent(enemyComponent);
        if (auto s = Game::GetInstance().GetCurrentState())
        {
            s->AddObject(enemy);
        }
    }
    if (ip.KeyPress(SPACE_KEY))
    {
        Character::Command c = Character::Command(Character::Command::DASH, {0,0});
        if (auto character = std::dynamic_pointer_cast<Character>(this->associated.GetComponent("Character").lock())) {
            character->Issue(c);
        }
    }
}

bool PlayerController::Is(std::string type)
{
    return type == "PlayerController";
}