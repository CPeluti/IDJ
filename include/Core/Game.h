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

    void Push(std::string state);

    void Run();

    void Menu();

    float GetDeltaTime();
    Vec2 GetWindowSize();

    inline float GetFps() {
        return this->fps;
    }

private:
    void CalculateDeltaTime();

    int frameStart;
    float dt;
    float fps;

    bool m_menuIsOpen;

    static Game *instance;
    std::unique_ptr<State> storedState;
    GPU_Target *m_gpuTarget;
    SDL_Renderer *renderer;
    std::stack<std::unique_ptr<State>> stateStack;
};