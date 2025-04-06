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


GPU_ShaderBlock load_shader_program(Uint32* p, const char* vertex_shader_file, const char* fragment_shader_file)
{
    Uint32 v, f;
    v = GPU_LoadShader(GPU_VERTEX_SHADER, vertex_shader_file);
    
    if(!v)
        GPU_LogError("Failed to load vertex shader (%s): %s\n", vertex_shader_file, GPU_GetShaderMessage());
    
    f = GPU_LoadShader(GPU_FRAGMENT_SHADER, fragment_shader_file);
    
    if(!f)
        GPU_LogError("Failed to load fragment shader (%s): %s\n", fragment_shader_file, GPU_GetShaderMessage());
    
    *p = GPU_LinkShaders(v, f);
    
    if(!*p)
    {
		GPU_ShaderBlock b = {-1, -1, -1, -1};
        GPU_LogError("Failed to link shader program (%s + %s): %s\n", vertex_shader_file, fragment_shader_file, GPU_GetShaderMessage());
        return b;
    }
    
	{
		GPU_ShaderBlock block = GPU_LoadShaderBlock(*p, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
		GPU_ActivateShaderProgram(*p, &block);

		return block;
	}
}

void prepare_mask_shader(Uint32 shader, GPU_Image* image, GPU_Image* mask_image)
{
    if(image != NULL)
    {
        GPU_SetUniformf(GPU_GetUniformLocation(shader, "resolution_x"), image->w);
        GPU_SetUniformf(GPU_GetUniformLocation(shader, "resolution_y"), image->h);
    }
    if(mask_image != NULL)
    {
        GPU_SetUniformf(GPU_GetUniformLocation(shader, "mask_resolution_x"), mask_image->w);
        GPU_SetUniformf(GPU_GetUniformLocation(shader, "mask_resolution_y"), mask_image->h);
    }
    
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "offset_x"), 0.0f);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "offset_y"), 0.0f);
    
    GPU_SetShaderImage(mask_image, GPU_GetUniformLocation(shader, "mask_tex"), 1);
}

void prepare_marching_ants_shader(Uint32 shader, GPU_Target* screen, GPU_Image* image)
{
    if(image != NULL)
    {
        GPU_SetUniformf(GPU_GetUniformLocation(shader, "resolution_x"), image->w);
        GPU_SetUniformf(GPU_GetUniformLocation(shader, "resolution_y"), image->h);
    }
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "screen_w"), screen->w);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "screen_h"), screen->h);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "zoom"), 1.0f);
}

void update_color_shader(float r, float g, float b, float a, int color_loc)
{
    LOG_INFO(r);
    float fcolor[4] = {r, g, b, a};
    GPU_SetUniformfv(color_loc, 4, 1, fcolor);
}

void update_vertex_color_shader(float t, float vertex_colors[24])
{
    for(int i = 0; i < 6; ++i)
    {
        vertex_colors[i] = 0.5f + 0.5f*(1 + sin(t + (i)))/2;
        vertex_colors[i+1] = 0.5f + 0.5f*(1 + sin(t + (i+1)))/2;
        vertex_colors[i+2] = 0.5f + 0.5f*(1 + sin(t + (i+2)))/2;
        vertex_colors[i+3] = 1.0f;
    }
}

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
        // this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
        this->m_gpuTarget = GPU_Init(width, height, GPU_DEFAULT_INIT_FLAGS);
        // this->renderer = SDL_CreateRenderer(this->window, -1, (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE));
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
    // SDL_DestroyRenderer(this->renderer);
    // SDL_DestroyWindow(this->window);
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
    Uint32 color_shader;
    GPU_Image* image = GPU_LoadImage("resources/img/NPC.png");
    GPU_ShaderBlock color_block = load_shader_program(&color_shader, "resources/shaders/common.vert", "resources/shaders/color.frag");
    int color_loc = GPU_GetUniformLocation(color_shader, "myColor");

    Uint32 mask_shader;
    GPU_ShaderBlock mask_block = load_shader_program(&mask_shader, "resources/shaders/common.vert", "resources/shaders/alpha_mask.frag");
    GPU_Image* mask_image = GPU_LoadImage("resources/img/NPC.png");
    prepare_mask_shader(mask_shader, image, mask_image);
    
    Uint32 marching_ants_shader;
    GPU_ShaderBlock marching_ants_block = load_shader_program(&marching_ants_shader, "resources/shaders/common.vert", "resources/shaders/marching_ants.frag");
    prepare_marching_ants_shader(marching_ants_shader, m_gpuTarget, image);
    int marching_ants_time_loc = GPU_GetUniformLocation(marching_ants_shader, "time");

    InputManager& inputManager = InputManager::GetInstance();
    float t;
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
        t = SDL_GetTicks()/1000.0f;
        GPU_ActivateShaderProgram(color_shader, &color_block);
        update_color_shader((1+sin(t))/2, (1+sin(t+1))/2, (1+sin(t+2))/2, 1.0f, color_loc);
        GPU_Blit(image, NULL, m_gpuTarget, m_gpuTarget->w/4, m_gpuTarget->h/4);
        GetCurrentState().Update(dt);
        GetCurrentState().Render();
        GPU_Flip(this->m_gpuTarget);
        // SDL_Delay(33);
        SDL_Delay(16);
    }
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    return;
}