#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#define INCLUDE_SDL_TTF
#include <string>
#include <iostream>
#include "Core/SDL_include.h"
#include "Core/Game.h"
#include "Core/Resources.h"
#include "Core/InputManager.h"
#include "Core/Log.h"
#include <cstdlib>
#include <ctime>

Game *Game::instance = nullptr;

Game::Game(std::string title, int width, int height): stateStack()
{
    float font_scale = 1;
    srand(time(NULL));
    if (this->instance == nullptr)
    {
        Log::Init();
        
        this->instance = this;
        if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
            SDL_Log("Failed to start SDL");
            SDL_Log(SDL_GetError());
        }
        if(!Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3)){
            SDL_Log("Failed to start SDL_Mixer");
            SDL_Log(SDL_GetError());
        }
        SDL_AudioSpec spec = {MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, MIX_DEFAULT_FREQUENCY};
        if(!Mix_OpenAudio(0, &spec)){
            SDL_Log("Failed open mix audio");
            SDL_Log(SDL_GetError());
        }
        if(!TTF_Init()){
            SDL_Log("Failed to init ttf");
            SDL_Log(SDL_GetError());
        }
        Mix_AllocateChannels(32);
        this->window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL);
        if(window==NULL){
            SDL_Log(SDL_GetError());
        }
        this->renderer = SDL_CreateRenderer(this->window, NULL);
        this->storedState = nullptr;

        frameStart = SDL_GetTicks();
        dt = 0;
    }
}
Game::~Game()
{
    if(storedState != nullptr){
        delete storedState;
    }
    stateStack = std::stack<std::unique_ptr<State>>();
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void Game::Push(State* state){
    storedState = state;
}

SDL_Renderer *Game::GetRenderer()
{
    return this->renderer;
}
State &Game::GetCurrentState()
{
    return *stateStack.top();
}

Vec2 Game::GetWindowSize(){
    int x;
    int y;
    SDL_GetWindowSize(window, &x, &y);
    return Vec2(x,y); 
}

Game &Game::GetInstance()
{
    if (Game::instance == nullptr)
    {
        new Game("190085312", 1200, 900);
    }
    return *Game::instance;
}

void Game::CalculateDeltaTime(){
    int oldFrame = frameStart;
    frameStart = SDL_GetTicks();
    dt = (frameStart-oldFrame)/1000.0;
}

float Game::GetDeltaTime(){
    return dt;
}

void Game::Run()
{
    InputManager& inputManager = InputManager::GetInstance();
    if(storedState != nullptr){
        stateStack.emplace(storedState);
        storedState = nullptr;
        GetCurrentState().Start();
    }
    while (!GetCurrentState().QuitRequested() && !stateStack.empty())
    {
        if(GetCurrentState().PopRequested()){
            stateStack.pop();
            if(!stateStack.empty()){
                GetCurrentState().Resume();
            }
            
        }
        if(storedState != nullptr){
            stateStack.emplace(storedState);
            GetCurrentState().Start();
            storedState = nullptr;
        }

        CalculateDeltaTime();
        inputManager.Update();
        GetCurrentState().Update(dt);
        GetCurrentState().Render();
        SDL_RenderPresent(renderer);
        // SDL_SetRenderDrawColor(renderer, bg.r * 255, bg.g * 255, bg.b * 255, bg.a * 255);
        // SDL_Delay(33);
        SDL_Delay(16);
    }
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    return;
}