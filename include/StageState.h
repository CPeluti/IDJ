#pragma once
#include "State.h"
#include "TileSet.h"
#include "GameObject.h"
#include "Music.h"
#include "WaveSpawner.h"

class StageState : public State{
    public:
        StageState();
        ~StageState();

        void LoadAssets();
        void Update(float dt);
        void Render();

        void Start();
        void Resume();
        void Pause();
        
    private: 
        TileSet* tileSet;
        Music backgroundMusic;
        std::weak_ptr<GameObject> player;
        std::weak_ptr<GameObject> spawner;
    };