#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#define INCLUDE_SDL_TTF
#include <string>
#include <iostream>
#include "Core/SDL_include.h"
#include "Core/Game.h"
#include "Core/Resources.h"
#include "Core/InputManager.h"
#include <cstdlib>
#include <ctime>
#define NK_SDL_RENDERER_IMPLEMENTATION
#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "Core/nuklear.h"
#include "Core/nuklear_sdl_renderer.h"

Game *Game::instance = nullptr;

Game::Game(std::string title, int width, int height): stateStack()
{
    float font_scale = 1;
    srand(time(NULL));
    if (this->instance == nullptr)
    {
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
            this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
            this->renderer = SDL_CreateRenderer(this->window, -1, (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE));
            this->storedState = nullptr;
            this->ctx = nk_sdl_init(window, renderer);

            {
                struct nk_font_atlas *atlas;
                struct nk_font_config config = nk_font_config(0);
                struct nk_font *font;
        
                /* set up the font atlas and add desired font; note that font sizes are
                 * multiplied by font_scale to produce better results at higher DPIs */
                nk_sdl_font_stash_begin(&atlas);
                font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
                /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14 * font_scale, &config);*/
                /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16 * font_scale, &config);*/
                /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13 * font_scale, &config);*/
                /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12 * font_scale, &config);*/
                /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10 * font_scale, &config);*/
                /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13 * font_scale, &config);*/
                nk_sdl_font_stash_end();
        
                /* this hack makes the font appear to be scaled down to the desired
                 * size and is only necessary when font_scale > 1 */
                font->handle.height /= font_scale;
                /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
                nk_style_set_font(ctx, &font->handle);
            }

            frameStart = SDL_GetTicks();
            dt = 0;
    }
}
Game::~Game()
{
    if(storedState != nullptr){
        delete storedState;
    }
    nk_sdl_shutdown();
    stateStack = std::stack<std::unique_ptr<State>>();
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
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

nk_context* Game::GetContext(){
    return this->ctx;
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
    nk_colorf bg;
    nk_style* s  = &ctx->style;
    InputManager& inputManager = InputManager::GetInstance();
    if(storedState != nullptr){
        stateStack.emplace(storedState);
        storedState = nullptr;
        GetCurrentState().Start();
    }
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
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
        nk_style_push_color(ctx, &s->window.background, nk_rgba(0,0,0,0));
        nk_style_push_style_item(ctx, &s->window.fixed_background, nk_style_item_color(nk_rgba(0,0,0,0)));
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250), false))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        nk_style_pop_color(ctx);
        nk_style_pop_style_item(ctx);

        CalculateDeltaTime();
        inputManager.Update();
        GetCurrentState().Update(dt);
        GetCurrentState().Render();
        nk_sdl_render(NK_ANTI_ALIASING_ON);
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, bg.r * 255, bg.g * 255, bg.b * 255, bg.a * 255);
        // SDL_Delay(33);
        SDL_Delay(16);
    }
    Resources::ClearImages();
    Resources::ClearMusics();
    Resources::ClearSounds();
    return;
}