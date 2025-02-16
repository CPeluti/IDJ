#pragma once
#include "State.h"
#include "TileSet.h"
#include "Music.h"

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
};