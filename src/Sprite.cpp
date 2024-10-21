#define INCLUDE_SDL_IMAGE
#include <string>
#include "SDL_include.h"
#include <iostream>
#include "Game.h"
Sprite::Sprite(){
    texture = nullptr;
}
Sprite::Sprite(std::string file){
    texture = nullptr;
    Open(file);
}
Sprite::~Sprite(){
    if(IsOpen()){
        SDL_DestroyTexture(texture);
    }
}
void Sprite::Open(std::string file){
    if(!IsOpen()){
        SDL_DestroyTexture(texture);
    }
    texture = IMG_LoadTexture(Game::GetInstance().GetRenderer(), file.c_str());
    if(IsOpen()){
        std::cout << "Error loading texture: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    SetClip(0, 0, width, height);
}
void Sprite::SetClip(int x, int y, int w, int h){
    clipRect.x = x;
    clipRect.y = y;
    clipRect.w = w;
    clipRect.h = h;
}
void Sprite::Render(int x, int y){
    SDL_Rect dstRect;
    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = clipRect.w;
    dstRect.h = clipRect.h;
    SDL_RenderCopy(Game::GetInstance().GetRenderer(), texture, &clipRect, &dstRect);
}
int Sprite::GetWidth(){
    return width;
}
int Sprite::GetHeight(){
    return height;
}
bool Sprite::IsOpen(){
    return texture != nullptr;
}
