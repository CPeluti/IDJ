#pragma once
#include "Core/State.h"
#include "Core/TileSet.h"
#include "Core/GameObject.h"
#include "Core/Music.h"
#include "Core/Text.h"
#include "WaveSpawner.h"
#include "TypingSystem.h"

class StageState : public State
{
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
	std::weak_ptr<Text> fpsText;
    TileSet *tileSet;
    Music backgroundMusic;
    std::weak_ptr<GameObject> player;
    std::weak_ptr<GameObject> spawner;
};