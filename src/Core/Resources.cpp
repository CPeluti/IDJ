#include "SDL2/SDL_gpu.h"
#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#define INCLUDE_SDL_TTF
#include "Core/SDL_include.h"
#include "Core/Resources.h"
#include "Core/Game.h"
#include <iostream>

std::unordered_map<std::string, GPU_Image *> Resources::imageTable;
std::unordered_map<std::string, Mix_Music *> Resources::musicTable;
std::unordered_map<std::string, Mix_Chunk *> Resources::soundTable;
std::unordered_map<std::string, TTF_Font*> Resources::fontTable;
std::unordered_map<std::string, int> Resources::vertexTable;
std::unordered_map<std::string, int> Resources::fragmentTable;
std::unordered_map<std::pair<std::string, std::string>, int, pair_hash> Resources::shaderBlockTable;

GPU_Image *Resources::GetImage(std::string file)
{

    if (imageTable.find(file) == imageTable.end())
    {
        try
        {
            GPU_Image* texture = GPU_LoadImage(file.c_str());
            GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
            imageTable[file] = texture;
            return texture;
        }
        catch (std::string e)
        {
            std::cout << "Failed to load texture: " << e << std::endl;
        }
    }
    return imageTable[file];
}

void Resources::ClearImages()
{
    for (auto const &entry : imageTable)
    {
        try
        {
            GPU_FreeImage(entry.second);
        }
        catch (std::string e)
        {
            std::cout << "Failed to delete texture: " << e << std::endl;
        }
    }
    imageTable.clear();
}

Mix_Music *Resources::GetMusic(std::string file)
{

    if (musicTable.find(file) == musicTable.end())
    {
        try
        {
            Mix_Music *music = Mix_LoadMUS(file.c_str());
            musicTable[file] = music;
            return music;
        }
        catch (std::string e)
        {
            std::cout << "Failed to load music: " << e << std::endl;
        }
    }
    return musicTable[file];
}

void Resources::ClearMusics()
{
    for (auto const &entry : musicTable)
    {
        try
        {
            Mix_FreeMusic(entry.second);
        }
        catch (std::string e)
        {
            std::cout << "Failed to delete music: " << e << std::endl;
        }
    }
    musicTable.clear();
}

Mix_Chunk *Resources::GetSound(std::string file)
{

    if (soundTable.find(file) == soundTable.end())
    {
        try
        {
            Mix_Chunk *sound = Mix_LoadWAV(file.c_str());
            soundTable[file] = sound;
            return sound;
        }
        catch (std::string e)
        {
            std::cout << "Failed to load sound: " << e << std::endl;
        }
    }
    return soundTable[file];
}

void Resources::ClearSounds()
{
    for (auto const &entry : soundTable)
    {
        try
        {
            Mix_FreeChunk(entry.second);
        }
        catch (std::string e)
        {
            std::cout << "Failed to delete sound: " << e << std::endl;
        }
    }
    soundTable.clear();
}

TTF_Font *Resources::GetFont(std::string file, int ptSize)
{

    if (fontTable.find(file + std::to_string(ptSize)) == fontTable.end())
    {
        try
        {
            TTF_Font *font = TTF_OpenFont(file.c_str(), ptSize);
            fontTable[file + std::to_string(ptSize)] = font;
            return font;
        }
        catch (std::string e)
        {
            std::cout << "Failed to load font: " << e << std::endl;
        }
    }
    return fontTable[file + std::to_string(ptSize)];
}

void Resources::ClearFonts()
{
    for (auto const &entry : fontTable)
    {
        try
        {
            TTF_CloseFont(entry.second);
        }
        catch (std::string e)
        {
            std::cout << "Failed to delete font: " << e << std::endl;
        }
    }
    fontTable.clear();
}


int Resources::GetVertex(std::string file)
{

    if (vertexTable.find(file) == vertexTable.end())
    {
        try
        {
            int v = GPU_LoadShader(GPU_VERTEX_SHADER, file.c_str());
            vertexTable[file] = v;
            return v;
        }
        catch (std::string e)
        {
            std::cout << "Failed to load Shader: " << e << std::endl;
            return -1;
        }
    }
    return vertexTable[file];
}

int Resources::GetFragment(std::string file)
{

    if (fragmentTable.find(file) == fragmentTable.end())
    {
        try
        {
            int f = GPU_LoadShader(GPU_FRAGMENT_SHADER, file.c_str());
            fragmentTable[file] = f;
            return f;
        }
        catch (std::string e)
        {
            std::cout << "Failed to load Shader: " << e << std::endl;
            return -1;
        }
    }
    return fragmentTable[file];
}

int Resources::GetShaderBlock(std::string vertexFile, std::string fragmentFile)
{

    int v = GetVertex(vertexFile);
    int f = GetFragment(fragmentFile);
    if (!v || !f) return -1;
	auto key = std::make_pair(vertexFile, fragmentFile);
    if (shaderBlockTable.find(key) == shaderBlockTable.end())
    {
        try
        {
            int p = GPU_LinkShaders(v,f);
            shaderBlockTable[{vertexFile, fragmentFile}] = p;
            return p;
        }
        catch (std::string e)
        {
            std::cout << "Failed to load Shader: " << e << std::endl;
            return -1;
        }
    }
    return shaderBlockTable[{vertexFile, fragmentFile}];
}