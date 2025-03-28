#pragma once
#define INCLUDE_SDL
#include "SDL_include.h"
#include "State.h"
#include <string>
#include <stack>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
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
        nk_context* GetContext();
    private:
    
        void CalculateDeltaTime();
        
        int frameStart;
        float dt;
        
        static Game* instance;
        nk_context* ctx;
        State* storedState;
        SDL_Window* window;
        SDL_Renderer* renderer;
        std::stack<std::unique_ptr<State>> stateStack;
        
};