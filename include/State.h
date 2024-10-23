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
        bool QuitRequested ();
        void LoadAssets ();
        void Update (float dt);
        void Render ();
        void Run ();
        void AddObject(GameObject* object);
    private: 
        Music* music;
        bool quitRequested;
        std::vector<std::unique_ptr<GameObject>> objectArray;
};