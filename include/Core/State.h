#pragma once
#include "Sprite.h"
#include "Music.h"
#include "Collider.h"
#include "Collision.h"
#include "Log.h"
#include <vector>
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
    virtual std::weak_ptr<GameObject> AddObject(GameObject *object);

    bool PopRequested();
    bool QuitRequested();

    inline void registerCollider(std::string layer_name, Collider *collider)
    {
        if (m_collisionLayers.find(layer_name) == m_collisionLayers.end())
        {
            m_collisionLayers[layer_name] = std::vector<Collider *>();
        }
        m_collisionLayers[layer_name].push_back(collider);
    }

    inline void removeCollider(std::string layer_name, Collider *collider)
    {
        for (int i = 0; i < m_collisionLayers[layer_name].size(); i++)
        {
            if (m_collisionLayers[layer_name][i] == collider)
            {
                m_collisionLayers[layer_name].erase(m_collisionLayers[layer_name].begin() + i);
            }
        }
    }

    inline void checkCollisions()
    {
        for (auto &el : m_collisionLayers)
        {
            for (int i = 0; i < (int)(el.second.size()); i++)
            {
                Collider *colliderA = el.second[i];
                if (colliderA != nullptr)
                {
                    for (int j = i + 1; j < el.second.size(); j++)
                    {
                        Collider *colliderB = el.second[j];
                        if (colliderB != nullptr)
                        {
                            if (Collision::IsColliding(colliderA->box, colliderB->box, colliderA->getAngleDeg(), colliderB->getAngleDeg()))
                            {
                                // LOG_INFO(colliderA->GetEvent());
                                // LOG_INFO(colliderB->GetEvent());
                                colliderA->getAssociated()->subject.notify(*colliderB->GetEvent());
                                colliderB->getAssociated()->subject.notify(*colliderA->GetEvent());
                            }
                        }
                    }
                }
            }
        }
    }

protected:
    void StartArray();
    virtual void UpdateArray(float dt);
    virtual void RenderArray();

    bool popRequested;
    bool started;
    bool quitRequested;

    std::vector<std::shared_ptr<GameObject>> objectArray;
    std::map<std::string, std::vector<Collider *>> m_collisionLayers;
};