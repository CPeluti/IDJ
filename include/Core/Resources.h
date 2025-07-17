#pragma once
#include <string>
#include "SDL_gpu.h"
#define INCLUDE_SDL
#define INCLUDE_SDL_MIXER
#define INCLUDE_SDL_TTF
#include "SDL_include.h"
#include <unordered_map>
struct pair_hash {
    std::size_t operator()(const std::pair<std::string, std::string>& p) const {
        std::hash<std::string> hash_fn;
        return hash_fn(p.first) ^ (hash_fn(p.second) << 1); // Combinação simples
    }
};
class Resources{
    public:
        static GPU_Image* GetImage(std::string file);
        static void ClearImages();
        static Mix_Music* GetMusic(std::string file);
        static void ClearMusics();
        static Mix_Chunk* GetSound(std::string file);
        static void ClearSounds();
        static TTF_Font* GetFont(std::string file, int ptSize);
        static void ClearFonts();
        static int GetVertex(std::string file);
        static int GetFragment(std::string file);
        static int GetShaderBlock(std::string vertexFile, std::string fragmentFile);
        static void ClearShaders();

    private:
        static std::unordered_map<std::string, int> vertexTable;
        static std::unordered_map<std::string, int> fragmentTable;
        static std::unordered_map<std::string, GPU_Image*> imageTable;
        static std::unordered_map<std::string, Mix_Music*> musicTable;
        static std::unordered_map<std::string, Mix_Chunk*> soundTable;
        static std::unordered_map<std::string, TTF_Font*> fontTable;
        static std::unordered_map<std::pair<std::string, std::string>,int, pair_hash> shaderBlockTable;
};