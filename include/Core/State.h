#pragma once
#include "Sprite.h"
#include "Music.h"
#include "Collider.h"
#include "Collision.h"
#include "Log.h"
#include "TileMap.h"
#include <vector>
#include <set>
#include <memory>
// class GameObject;
#include "GameObject.h"
class State
{
public:
    State();
    virtual ~State();

    virtual void LoadAssets() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

    virtual void Start() = 0;
    virtual void Resume() = 0;
    virtual void Pause() = 0;

    virtual std::weak_ptr<GameObject> GetObjectPtr(GameObject *go);
    virtual std::weak_ptr<GameObject> AddObject(std::shared_ptr<GameObject> object);

    bool PopRequested();
    bool QuitRequested();

    void SetPopRequested(bool req)
    {
        popRequested = req;
    }

    inline void registerCollider(std::string layer_name, std::weak_ptr<Collider> collider)
    {
        if (m_collisionLayers.find(layer_name) == m_collisionLayers.end())
        {
            m_collisionLayers[layer_name] = std::vector<std::weak_ptr<Collider>>();
        }
        m_collisionLayers[layer_name].push_back(collider);
    }


    inline void removeCollider(std::string layer_name, Collider* collider)
    {
        for (int i = 0; i < m_collisionLayers[layer_name].size(); i++)
        {
            auto s = m_collisionLayers[layer_name][i].lock();
            if (!s || s.get() == collider)
            {
                m_collisionLayers[layer_name].erase(m_collisionLayers[layer_name].begin() + i);
            }
        }
    }


    inline void checkCollisions(float dt)
    {
        for (auto &el : m_collisionLayers)
        {
            for (int i = 0; i < (int)(el.second.size()); i++)
            {
                    auto colliderA = el.second[i].lock();
                    if (!colliderA)
						continue;
                    if(colliderA->disabled)
						continue;
                    for (int j = i + 1; j < el.second.size(); j++)
                    {
                        auto colliderB = el.second[j].lock();
                        if (!colliderB)
                            continue;
                        if (colliderB->disabled)
                            continue;
                        if(colliderA->GetTag() != colliderB->GetTag()){
                            auto r = Collision::IsColliding(colliderA, colliderB);
                            Collision::ResolveCollision(colliderA, colliderB, r);
                        }
                    }
            }
        }
    }

    inline std::weak_ptr<TileMap> GetTileMap()
    {
        return tileMap;
	}

    inline void SetTileMap(std::weak_ptr<TileMap> map)
    {
        tileMap = map;
    }

protected:
    void StartArray();
    virtual void UpdateArray(float dt);
    virtual void RenderArray();

    bool popRequested;
    bool started;
    bool quitRequested;
    float time;

	std::weak_ptr<TileMap> tileMap;

    std::vector<std::shared_ptr<GameObject>> objectArray;
    std::map<std::string, std::vector<std::weak_ptr<Collider>>> m_collisionLayers;
};