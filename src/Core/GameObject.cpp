#include "Core/GameObject.h"
#include "Core/Game.h"
#include "Core/TileMap.h"
#include <iostream>
// #include "Component.h"
GameObject::GameObject()
{
    isDead = false;
    started = false;
    angleDeg = 0;
    z=1;
}

GameObject::~GameObject()
{
    for(auto component = components.begin(); component != components.end();)
    {
        component = components.erase(component);
    }
}

void GameObject::Start(){
    started = true;
    for(auto iter : components){
        iter->Start();
    }
}

void GameObject::Update(float dt)
{
    this->box.MoveToPos(m_speed);
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        this->components[i]->Update(dt);
    }
}

void GameObject::Render()
{
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        // std::cout << "rendering " << this->components[i]->Is("TileMap");
        this->components[i]->Render();
    }
}

bool GameObject::IsDead()
{
    return this->isDead;
}

void GameObject::RequestDelete()
{
    this->isDead = true;
}

void GameObject::AddComponent(std::shared_ptr<Component> cpt)
{
    this->components.emplace_back(cpt);
    if(started){
        cpt->Start();
    }
}

void GameObject::RemoveComponent(std::weak_ptr<Component> cpt)
{
    for (int i = 0; i < (int)this->components.size();)
    {
        if (this->components[i] == cpt.lock())
        {
            this->components.erase(this->components.begin() + i);
        } else {
            i++;
        }
    }
}

std::weak_ptr<Component> GameObject::GetComponent(std::string type)
{
    if(type == "Animator"){
        std::cout << "teste";
    }
    for (int i = 0; i < (int)this->components.size(); i++)
    {
        if (this->components[i]->Is(type))
        {
            return this->components[i];
        }
    }
    return std::weak_ptr<Component>();
}

// void GameObject::NotifyCollision(GameObject& other)
// {
//     for (int i = 0; i < (int)this->components.size(); i++)
//     {
//         this->components[i]->NotifyCollision(other);
//     }
// }
Raycast GameObject::CastRaycast(Vec2 start, Vec2 end, float maxDistance, int layer)
{
    try {
	    std::shared_ptr<TileMap> tilemap = Game::GetInstance().GetCurrentState()->GetTileMap().lock();
        start = tilemap->GetPosInTileMap(start);
		end = tilemap->GetPosInTileMap(end);
        if(!tilemap) {
            //LOG_ERROR("TileMap not found in Raycast");
            return Raycast{ false, {0, 0}, false};
		}
        Vec2 direction = (end - start).normalized();
        Vec2 rayStepSize = { sqrt(1 + (direction.y / direction.x) * (direction.y / direction.x)), sqrt(1 + (direction.x / direction.y) * (direction.x / direction.y)) };

        Vec2 currentPos = { int(start.x), int(start.y) };

        Vec2 rayLength;

        Vec2 step;

        if (direction.x < 0)
        {
            step.x = -1;
            rayLength.x = (start.x - float(currentPos.x)) * rayStepSize.x;
        }
        else
        {
            step.x = 1;
            rayLength.x = (float(currentPos.x + 1) - start.x) * rayStepSize.x;
        }

        if (direction.y < 0)
        {
            step.y = -1;
            rayLength.y = (start.y - float(currentPos.y)) * rayStepSize.y;
        }
        else
        {
            step.y = 1;
            rayLength.y = (float(currentPos.y + 1) - start.y) * rayStepSize.y;
        }

        bool hasTile = false;
        float distance = 0.0f;
        while (!hasTile && distance < maxDistance/16)
        {
            if (rayLength.x < rayLength.y)
            {
                currentPos.x += step.x;
                distance = rayLength.x;
                rayLength.x += rayStepSize.x;
            }
            else
            {
                currentPos.y += step.y;
                distance = rayLength.y;
                rayLength.y += rayStepSize.y;
            }
            //bool t = tileSet->getColliderMetaData(tileMatrix[layer * mapWidth * mapHeight + currentPos.y * mapWidth + currentPos.x]);
            //LOG_INFO("currentpos: {} {}", int(currentPos.x), int(currentPos.y));

            auto maxX = std::max(int(start.x), int(end.x));
            auto maxY = std::max(int(start.y), int(end.y));

            auto minX = std::min(int(start.x), int(end.x));
            auto minY = std::min(int(start.y), int(end.y));
            int mapWidth = tilemap->GetWidth();
            int mapHeight = tilemap->GetHeight();
            bool isContained = int(currentPos.x) >= minX && int(currentPos.x) <= maxX && int(currentPos.y) >= minY && int(currentPos.y) <= maxY;
            if (!isContained) break;
            if (currentPos.x >= 0 && currentPos.x < mapWidth && currentPos.y >= 0 && currentPos.y < mapHeight)
            {
                int tileCode = tilemap->At(int(currentPos.x), int(currentPos.y), layer);
                if (tilemap->GetTileMetadata(tileCode)) {
                    hasTile = true;
                }
            }
            else {
                // Out of bounds, stop checking 
                //LOG_ERROR("Raycast out of bounds at position: ", currentPos.x, ",", currentPos.y);
            }
        }

        Vec2 hitPos;
        if (hasTile) {
            hitPos = start + direction * distance;
        }

        return Raycast{ hasTile, hitPos, distance >= maxDistance / 16 };

    }
    catch (const std::exception& e) {
        LOG_ERROR("Raycast error: ", e.what());
        return Raycast{ false, {0, 0} , false};
    }
}