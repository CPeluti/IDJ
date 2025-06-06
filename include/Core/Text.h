#pragma once
#define INCLUDE_SDL_TTF
#include "SDL_gpu.h"
#include "SDL_Include.h"
#include "GameObject.h"
#include "Component.h"
#include "Timer.h"
class Text : public Component
{
public:
    enum TextStyle
    {
        SOLID,
        SHADED,
        BLENDED
    };
    Text(
        GameObject &associated,
        std::string fontFile,
        int fontSize,
        TextStyle style,
        std::string text,
        SDL_Color color,
        int blink,
        bool cameraFollower = false);
    ~Text();
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    void SetText(std::string text);
    void SetColor(SDL_Color color);
    void SetStyle(TextStyle style);
    void SetFontFile(std::string fontFile);
    void SetFontSize(int fontSize);
    void RemakeTexture();

private:
    TTF_Font *font;
    GPU_Image *texture;
    std::string text;
    TextStyle style;
    std::string fontFile;
    int fontSize;
    SDL_Color color;
    Timer blink;
    bool appear;
    bool cameraFollower;
};