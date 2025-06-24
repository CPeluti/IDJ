#pragma once
#define INCLUDE_SDL
#include "SDL_include.h"
#include "State.h"
#include <string>
#include <stack>

class Game
{
public:
    Game(std::string title, int width, int height);
    ~Game();

    static Game &GetInstance();
    GPU_Target *GetGPUTarget();
    State* GetCurrentState();

    void Push(std::unique_ptr<State> state);

    void Run();

    float GetDeltaTime();
    Vec2 GetWindowSize();

private:
    void CalculateDeltaTime();

    int frameStart;
    float dt;

    static Game *instance;
    std::unique_ptr<State> storedState;
    GPU_Target *m_gpuTarget;
    SDL_Renderer *renderer;
    std::stack<std::unique_ptr<State>> stateStack;
};