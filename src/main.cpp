#define INCLUDE_SDL
#define MAX_MEMORY 16384
#include "Core/SDL_include.h"
#include "Core/Game.h"
#include "Game/TitleState.h"
#include <iostream>
int main(int argc, char* argv[]){
    SDL_Log("Starting game"); 
    Game& game = Game::GetInstance();
    game.Push(new TitleState());
    game.Run();
    return 0;
}