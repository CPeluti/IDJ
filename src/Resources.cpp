#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#include "Resources.h"
#include "SDL_include.h"
#include "Game.h"
#include <iostream>


std::unordered_map<std::string, SDL_Texture*> Resources::imageTable;
std::unordered_map<std::string, Mix_Music*> Resources::musicTable;
std::unordered_map<std::string, Mix_Chunk*> Resources::soundTable;

SDL_Texture* Resources::GetImage(std::string file){
    
    if(imageTable.find(file) == imageTable.end()){
        try{
            SDL_Texture* texture = IMG_LoadTexture(Game::GetInstance().GetRenderer(), file.c_str());
            imageTable[file] = texture;
            return texture;
        } catch (std::string e){
            std::cout << "Failed to load texture: " << e << std::endl; 
        }
    }
    return imageTable[file];
}

void Resources::ClearImages(){
    for(auto const &entry : imageTable){
        try {
            SDL_DestroyTexture(entry.second);
        } catch (std::string e){
            std::cout << "Failed to delete texture: " << e << std::endl; 
        }
    }
    imageTable.clear();
}

Mix_Music* Resources::GetMusic(std::string file){
    
    if(musicTable.find(file) == musicTable.end()){
        try{
            Mix_Music* music = Mix_LoadMUS(file.c_str());
            musicTable[file] = music;
            return music;
        } catch (std::string e){
            std::cout << "Failed to load music: " << e << std::endl; 
        }
    }
    return musicTable[file];
}

void Resources::ClearMusics(){
    for(auto const &entry : musicTable){
        try {
            Mix_FreeMusic(entry.second);
        } catch (std::string e){
            std::cout << "Failed to delete music: " << e << std::endl; 
        }
    }
    musicTable.clear();
}


Mix_Chunk* Resources::GetSound(std::string file){
    
    if(soundTable.find(file) == soundTable.end()){
        try{
            Mix_Chunk* sound = Mix_LoadWAV(file.c_str());
            soundTable[file] = sound;
            return sound;
        } catch (std::string e){
            std::cout << "Failed to load sound: " << e << std::endl; 
        }
    }
    return soundTable[file];
}

void Resources::ClearSounds(){
    for(auto const &entry : soundTable){
        try {
            Mix_FreeChunk(entry.second);
        } catch (std::string e){
            std::cout << "Failed to delete sound: " << e << std::endl; 
        }
    }
    soundTable.clear();
}

