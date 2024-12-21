#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#include <string>
#include <iostream>
#include "SDL_include.h"
#include "Game.h"
#include "Resources.h"
#include "InputManager.h"

Game *Game::instance = nullptr;

Game::Game(std::string title, int width, int height)
{
    if (this->instance == nullptr)
    {
        this->instance = this;
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
        Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
        Mix_AllocateChannels(32);
        this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
        this->renderer = SDL_CreateRenderer(this->window, -1, (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE));
        this->state = new State();
        frameStart = SDL_GetTicks();
        dt = 0;
    }
}
Game::~Game()
{
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
SDL_Renderer *Game::GetRenderer()
{
    return this->renderer;
}
State &Game::GetState()
{
    return *state;
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
    while (!state->QuitRequested())
    {
        CalculateDeltaTime();
        inputManager.Update();
        state->Update(dt);
        state->Render();
        SDL_RenderPresent(renderer);
        // SDL_Delay(33);
        SDL_Delay(8);
    }
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    return;
}