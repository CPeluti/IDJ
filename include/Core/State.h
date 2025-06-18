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

    void SetPopRequested(bool req)
    {
        popRequested = req;
    }

    inline void registerCollider(std::string layer_name, Collider *collider)
    {
        if (m_collisionLayers.find(layer_name) == m_collisionLayers.end())
        {
            m_collisionLayers[layer_name] = std::vector<Collider *>();
        }
        m_collisionLayers[layer_name].push_back(collider);
    }

    inline void registerPhysicsCollider(std::string layer_name, Collider *collider)
    {
        if (m_physicsLayers.find(layer_name) == m_physicsLayers.end())
        {
            m_physicsLayers[layer_name] = std::map<std::string, std::vector<Collider *>>();
        }
        if(m_physicsLayers[layer_name].find(collider->GetTag()) == m_physicsLayers[layer_name].end()){

            m_physicsLayers[layer_name][collider->GetTag()] = std::vector<Collider *>();
        }
        m_physicsLayers[layer_name][collider->GetTag()].push_back(collider);
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

        inline void removePhysicsCollider(std::string layer_name, Collider *collider)
    {
        for (int i = 0; i < m_physicsLayers[layer_name].size(); i++)
        {
            if (m_physicsLayers[layer_name][collider->GetTag()][i] == collider)
            {
                m_physicsLayers[layer_name][collider->GetTag()].erase(m_physicsLayers[layer_name][collider->GetTag()].begin() + i);
            }
        }
    }

    inline void checkCollisions(float dt)
    {

        time = 0;
        for(auto &el : m_physicsLayers){
            //iter on tags
            for(auto tag : el.second){
                //iter on the elements with the same tag
                for (int i = 0; i < (int)(tag.second.size()); i++)
                {
                    //base collider (the one being checked)
                    Collider *colliderA = tag.second[i];
                    if (colliderA != nullptr)
                    {
                        //iter on the tags to check collision between different tags
                        for(auto tag2 : el.second)
                        {
                            if(tag == tag2){
                                continue;
                            }
                            //secondary collider (the one that is on a different tag vector)
                            for(int i = 0; i<tag2.second.size(); i++){
                                Collider *colliderB = tag2.second[i];
                                if (colliderB != nullptr)
                                {
                                    if(colliderA->GetTag() != colliderB->GetTag()){
                                        GameObject* goA = colliderA->getAssociated();
                                        GameObject* goB = colliderB->getAssociated();
                                        if (Collision::IsColliding(colliderA->box, colliderB->box, colliderA->getAngleDeg(), colliderB->getAngleDeg(), goA->GetSpeed(), goB->GetSpeed())){
                                            // colliderA->getAssociated()->subject.notify(*colliderB->GetEvent());
                                            // colliderB->getAssociated()->subject.notify(*colliderA->GetEvent());
                                            goA->SetSpeed(goA->GetSpeed()*-1);
                                            goB->SetSpeed(goB->GetSpeed()*-1);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
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
    float time;

    std::vector<std::shared_ptr<GameObject>> objectArray;
    std::map<std::string, std::vector<Collider *>> m_collisionLayers;
    std::map<std::string, std::map<std::string,std::vector<Collider *>>> m_physicsLayers;
};