#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"

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

Game::Game(std::string title, int width, int height): stateStack()
{
    float font_scale = 1;
    srand(time(NULL));
    if (this->instance == nullptr)
    {
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
        Log::Init();
        this->instance = this;
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)){
            SDL_Log("Failed to start SDL");
            SDL_Log(SDL_GetError());
        }
        if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)){
            SDL_Log("Failed to start SDL_Image");
            SDL_Log(SDL_GetError());
        }
        if(Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3)){
            SDL_Log("Failed to start SDL_Mixer");
            SDL_Log(SDL_GetError());
        }
        if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)){
            SDL_Log("Failed open mix audio");
            SDL_Log(SDL_GetError());
        }
        if(TTF_Init()){
            SDL_Log("Failed to init ttf");
            SDL_Log(SDL_GetError());
        }
        Mix_AllocateChannels(32);
        this->m_gpuTarget = GPU_Init(width, height, GPU_DEFAULT_INIT_FLAGS);
        this->storedState = nullptr;
        frameStart = SDL_GetTicks();
        dt = 0;
        // Setup Dear ImGui context

        SDL_GLContext& gl_context = m_gpuTarget->context->context;
        SDL_Window* window = SDL_GetWindowFromID(m_gpuTarget->context->windowID);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 460");
            // Setup style
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
    // SDL_DestroyRenderer(this->renderer);
    // SDL_DestroyWindow(this->window);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    GPU_FreeTarget(m_gpuTarget);
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::Push(State* state){
    storedState = state;
}

GPU_Target *Game::GetGPUTarget()
{
    return this->m_gpuTarget;
}
State &Game::GetCurrentState()
{
    return *stateStack.top();
}

nk_context* Game::GetContext(){
    return this->ctx;
}

Vec2 Game::GetWindowSize(){

    return Vec2(m_gpuTarget->w,m_gpuTarget->h); 
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
    ImGui::StyleColorsClassic();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    InputManager& inputManager = InputManager::GetInstance();
    float t;
    if(storedState != nullptr){
        stateStack.emplace(storedState);
        storedState = nullptr;
        GetCurrentState().Start();
    }
    GPU_ActivateShaderProgram(0, NULL);
    while (!GetCurrentState().QuitRequested() && !stateStack.empty())
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow(); // Show demo window! :)
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
        t = SDL_GetTicks()/1000.0f;
        GetCurrentState().Update(dt);
        GetCurrentState().Render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GPU_Flip(this->m_gpuTarget);
        SDL_Delay(16);
    }
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    
    return;
}