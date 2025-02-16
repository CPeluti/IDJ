#pragma once
#include "Sprite.h"
#include "Music.h"
#include <vector>
#include <memory>
// class GameObject;
#include "GameObject.h"
class State{
    public:
        State();
        virtual ~State();

        virtual void LoadAssets() = 0;
        virtual void Update(float dt) = 0;
        virtual void Render() = 0;

        virtual void Start () = 0;
        virtual void Resume () = 0;
        virtual void Pause () = 0;
        
        virtual std::weak_ptr<GameObject> GetObjectPtr(GameObject* go);
        virtual std::weak_ptr<GameObject> AddObject(GameObject* object);

        bool PopRequested();
        bool QuitRequested();

        
    protected: 
        void StartArray();
        virtual void UpdateArray(float dt);
        virtual void RenderArray();
        
        bool popRequested;
        bool started;
        bool quitRequested;

        std::vector<std::shared_ptr<GameObject>> objectArray;
};