#define INCLUDE_SDL
#include "SDL_include.h"
#include <iostream>
#include "InputManager.h"
InputManager& InputManager::GetInstance(){
    static InputManager inputManager;
    return inputManager;
}

InputManager::InputManager():quitRequested(false), updateCounter(0), mouseX(0), mouseY(0), mouseState{false,false,false,false,false,false}, mouseUpdate{0}{}

void InputManager::Update(){
    quitRequested = false;

    SDL_GetMouseState(&this->mouseX, &this->mouseY);
    updateCounter = SDL_GetTicks();
    SDL_Event event;
    if(SDL_PollEvent(&event)){
        std::cout << event.type << std::endl;
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if(!event.key.repeat){
                keyState[event.key.keysym.sym] = true;
                keyUpdate[event.key.keysym.sym] = updateCounter;
            }
            break;
        case SDL_KEYUP:
            keyState[event.key.keysym.sym] = false;
            keyUpdate[event.key.keysym.sym] = updateCounter;
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseState[event.button.button] = true;
            mouseUpdate[event.button.button] = updateCounter;
            break;
        case SDL_MOUSEBUTTONUP:
            mouseState[event.button.button] = false;
            mouseUpdate[event.button.button] = updateCounter;
            break;
        case SDL_QUIT:
            quitRequested = true;
            break;
        default:
            break;
        }
    }

}

bool InputManager::KeyPress(int key){
    if(keyState.find(key) != keyState.end()){
        return keyState[key] && (updateCounter == keyUpdate[key]);
    }
    return false;
}

bool InputManager::KeyRelease(int key){
    if(keyState.find(key) != keyState.end()){
        return !keyState[key] && (updateCounter == keyUpdate[key]);
    }
    return false;
}

bool InputManager::IsKeyDown(int key){
    if(keyState.find(key) != keyState.end()){
        return keyState[key];
    }
}

bool InputManager::MousePress(int button){
    return mouseState[button] && (updateCounter == mouseUpdate[button]);
}

bool InputManager::MouseRelease(int button){
    return !mouseState[button] && (updateCounter == mouseUpdate[button]);
}

bool InputManager::IsMouseDown(int button){
    return mouseState[button];
}

int InputManager::GetMouseX(){
    return mouseX;
}

int InputManager::GetMouseY(){
    return mouseY;
}

bool InputManager::QuitRequested(){
    return quitRequested;
}