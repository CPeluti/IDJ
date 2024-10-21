#define INCLUDE_SDL
#include "SDL_include.h"
#include <Game.h>
#include <iostream>
int main(int argv, char** args){
    SDL_Log("Starting game"); 
    Game& game = Game::GetInstance();
    game.Run();
    return 0;
}