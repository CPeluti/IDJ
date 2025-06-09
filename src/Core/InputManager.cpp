#define INCLUDE_SDL
#include "Core/SDL_include.h"
#include <iostream>
#include "Core/InputManager.h"
#include "Core/Game.h"
#include "Core/Camera.h"
#include "Game/TitleState.h"

#include "Game/TypingSystem.h"

InputManager &InputManager::GetInstance()
{
    static InputManager inputManager;
    return inputManager;
}

InputManager::InputManager() : quitRequested(false), updateCounter(0), mouseX(0), mouseY(0), mouseState{false, false, false, false, false, false}, mouseUpdate{0} {}

void InputManager::Update()
{
    quitRequested = false;
    SDL_GetMouseState(&this->mouseX, &this->mouseY);
    updateCounter = SDL_GetTicks();

    TypingSystem &ts = TypingSystem::GetInstance();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (ts.IsTypingMode())
        {
            ts.HandleInput(event);
        }
        else
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                if (!event.key.repeat)
                {
                    keyState[event.key.keysym.scancode] = true;
                    keyUpdate[event.key.keysym.scancode] = updateCounter;
                }
                break;
            case SDL_KEYUP:
                keyState[event.key.keysym.scancode] = false;
                keyUpdate[event.key.keysym.scancode] = updateCounter;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouseState[event.button.button] = true;
                mouseUpdate[event.button.button] = updateCounter;
                break;
            case SDL_MOUSEBUTTONUP:
                mouseState[event.button.button] = false;
                mouseUpdate[event.button.button] = updateCounter;
                break;
            default:
                break;
            }
        }

        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_LSHIFT)
            {
                if (ts.IsTypingMode())
                {
                    ts.SwitchTypingMode();
                }
                else
                {
                    ts.ResetSubmission();
                    ts.SwitchTypingMode();
                }
            }
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                // popRequested = true;
                Game::GetInstance().GetCurrentState().SetPopRequested(true);
                TitleState *titleStage = new TitleState();
                Game::GetInstance().Push(titleStage);
            }
            break;
        case SDL_QUIT:
            quitRequested = true;
            break;
        default:
            break;
        }
    }
}

bool InputManager::KeyPress(int key)
{
    if (keyState.find(key) != keyState.end())
    {
        return keyState[key] && (updateCounter == keyUpdate[key]);
    }
    return false;
}

bool InputManager::KeyRelease(int key)
{
    if (keyState.find(key) != keyState.end())
    {
        return !keyState[key] && (updateCounter == keyUpdate[key]);
    }
    return false;
}

bool InputManager::IsKeyDown(int key)
{
    if (keyState.find(key) != keyState.end())
    {
        return keyState[key];
    }
    return false;
}

bool InputManager::MousePress(int button)
{
    return mouseState[button] && (updateCounter == mouseUpdate[button]);
}

bool InputManager::MouseRelease(int button)
{
    return !mouseState[button] && (updateCounter == mouseUpdate[button]);
}

bool InputManager::IsMouseDown(int button)
{
    return mouseState[button];
}

int InputManager::GetMouseX()
{
    return mouseX + Camera::pos.x;
}

int InputManager::GetMouseY()
{
    return mouseY + Camera::pos.y;
}

bool InputManager::QuitRequested()
{
    return quitRequested;
}