#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "SDL2/SDL_gpu.h"
#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#define INCLUDE_SDL_TTF
#include "Core/SDL_include.h"
#include "Core/Game.h"
#include "Core/Resources.h"
#include "Core/InputManager.h"
#include "Core/Log.h"

void update_marching_ants_shader(float t, int time_loc)
{
    GPU_SetUniformf(time_loc, t);
}

void free_shader(Uint32 p)
{
    GPU_FreeShaderProgram(p);
}

Game *Game::instance = nullptr;

Game::Game(std::string title, int width, int height) : stateStack()
{
    float font_scale = 1;
    srand(time(NULL));
    if (this->instance == nullptr)
    {
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
        Log::Init();
        this->instance = this;
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
        {
            SDL_Log("Failed to start SDL");
            SDL_Log(SDL_GetError());
        }
        if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF))
        {
            SDL_Log("Failed to start SDL_Image");
            SDL_Log(SDL_GetError());
        }
        if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3))
        {
            SDL_Log("Failed to start SDL_Mixer");
            SDL_Log(SDL_GetError());
        }
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))
        {
            SDL_Log("Failed open mix audio");
            SDL_Log(SDL_GetError());
        }
        if (TTF_Init())
        {
            SDL_Log("Failed to init ttf");
            SDL_Log(SDL_GetError());
        }
        Mix_AllocateChannels(128);
        GPU_SetPreInitFlags(GPU_INIT_DISABLE_VSYNC);
        this->m_gpuTarget = GPU_Init(width, height, GPU_DEFAULT_INIT_FLAGS);
        this->storedState = nullptr;
        frameStart = SDL_GetTicks();
        dt = 0;
        // Setup Dear ImGui context

        SDL_GLContext &gl_context = m_gpuTarget->context->context;
        SDL_Window *window = SDL_GetWindowFromID(m_gpuTarget->context->windowID);
    }
}
Game::~Game()
{
    if (storedState != nullptr)
    {
        storedState.reset();
    }
    stateStack = std::stack<std::unique_ptr<State>>();
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    // SDL_DestroyRenderer(this->renderer);
    // SDL_DestroyWindow(this->window);
    GPU_FreeTarget(m_gpuTarget);
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::Push(std::unique_ptr<State> state)
{
    storedState = std::move(state);
}

GPU_Target *Game::GetGPUTarget()
{
    return this->m_gpuTarget;
}
State* Game::GetCurrentState()
{
    if (stateStack.empty()) {
        return nullptr;  // Ou lance uma exceção
    }
    return stateStack.top().get();
}

Vec2 Game::GetWindowSize()
{

    return Vec2(m_gpuTarget->w, m_gpuTarget->h);
}

Game &Game::GetInstance()
{
    if (Game::instance == nullptr)
    {
        new Game("190085312", 1920, 1080);
    }
    return *Game::instance;
}

void Game::CalculateDeltaTime()
{
    int oldFrame = frameStart;
    frameStart = SDL_GetTicks();
    dt = (frameStart - oldFrame) / 1000.0;
}

float Game::GetDeltaTime()
{
    return dt;
}

void Game::Menu() {
	LOG_INFO("Menu requested");
	m_menuIsOpen = !m_menuIsOpen;
    if (m_menuIsOpen) {
        Mix_Resume(-1);
        this->GetCurrentState()->Resume();
    } else {
        Mix_Pause(-1);
        this->GetCurrentState()->Pause();
    }
    return;
}

void Game::Run()
{
    int frames_drawn = 0;
    this->fps = 0.0f;
    int startTime = SDL_GetTicks();

    InputManager &inputManager = InputManager::GetInstance();
    float t;
    if (storedState != nullptr)
    {
        stateStack.emplace(std::move(storedState));
        storedState.reset();
        if(auto s = GetCurrentState())
            s->Start();
    }
    GPU_ActivateShaderProgram(0, NULL);
    while (!GetCurrentState()->QuitRequested() && !stateStack.empty())
    {
        if(auto s = GetCurrentState()){
            if (s->PopRequested())
            {
                auto aux = std::move(stateStack.top());
                stateStack.pop();
                if (!stateStack.empty())
                {
                    s->Resume();
                }
            }
            if (storedState != nullptr)
            {
                stateStack.emplace(std::move(storedState));
                s = GetCurrentState();
                s->Start();
                storedState.reset();
            }
            CalculateDeltaTime();
            inputManager.Update();
            //t = SDL_GetTicks() / 1000.0f;
            s->Update(dt);
            GPU_Clear(this->m_gpuTarget);
            s->Render();
            GPU_FlushBlitBuffer();
            GPU_Flip(this->m_gpuTarget);
            //SDL_Delay(16);
            frames_drawn++;
            int currentTime = SDL_GetTicks();
            int diff = currentTime - startTime;
            //fps_counter += diff;
            //prev_ticks = ticks_now;
            if (diff >= 1000) {
                this->fps = frames_drawn / (diff / 1000.0f);
                frames_drawn = 0;
                startTime = currentTime;
                //
             }
        }

    }
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();

    return;
}