#pragma once
#include <unordered_map>
#define INCLUDE_SDL
#include "SDL_include.h"

#define W_KEY               SDL_SCANCODE_W
#define A_KEY               SDL_SCANCODE_A
#define S_KEY               SDL_SCANCODE_S
#define D_KEY               SDL_SCANCODE_D

#define LEFT_ARROW_KEY      SDL_SCANCODE_LEFT
#define RIGHT_ARROW_KEY     SDL_SCANCODE_RIGHT
#define UP_ARROW_KEY        SDL_SCANCODE_UP
#define DOWN_ARROW_KEY      SDL_SCANCODE_DOWN
#define ESCAPE_KEY          SDL_SCANCODE_ESCAPE
#define LEFT_MOUSE_BUTTON   SDL_BUTTON_LEFT
#define SPACE_KEY           SDL_SCANCODE_SPACE
class InputManager{
    public:
        void Update ();
        bool KeyPress (int key);
        bool KeyRelease (int key);
        bool IsKeyDown (int key);
        bool MousePress (int button);
        bool MouseRelease (int button);
        bool IsMouseDown (int button);
        int GetMouseX ();
        int GetMouseY ();
        bool QuitRequested ();
        static InputManager& GetInstance ();
    private:

        InputManager ();
        // ~InputManager ();

        bool quitRequested;
        int updateCounter;
        int mouseX;
        int mouseY;
        bool mouseState [6];
        int mouseUpdate [6];
        std::unordered_map<int, bool> keyState;
        std::unordered_map<int, int> keyUpdate;
};