#pragma once
#define INCLUDE_SDL
#include "SDL_include.h"
#include "State.h"
#include <string>
#include <stack>

class Game {
    public:
        Game(std::string title, int width, int height);
        ~Game();
        
        static Game& GetInstance();
        SDL_Renderer* GetRenderer();
        State& GetCurrentState();
        
        void Push(State* state);
        
        void Run();
        
        float GetDeltaTime();
        Vec2 GetWindowSize();
    private:
    
        void CalculateDeltaTime();
        
        int frameStart;
        float dt;
        
        static Game* instance;
        State* storedState;
        SDL_Window* window;
        SDL_Renderer* renderer;
        std::stack<std::unique_ptr<State>> stateStack;
        
};