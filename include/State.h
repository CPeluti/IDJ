#pragma once
#include "Sprite.h"
#include "Music.h"
#include <vector>
#include <memory>
// class GameObject;
#include "GameObject.h"
class State{
    public:
        State ();
        ~State ();
        void Start();
        bool QuitRequested ();
        void LoadAssets ();
        void Update (float dt);
        void Render ();
        void Run ();
        std::weak_ptr<GameObject> AddObject(GameObject* object);
        std::weak_ptr<GameObject> GetObjectPtr(GameObject* go);
    private: 
        bool started;
        std::vector<std::shared_ptr<GameObject>> objectArray;
        Music* music;
        bool quitRequested;
};