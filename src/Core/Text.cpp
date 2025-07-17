#include "Core/Text.h"
#include "Core/Resources.h"
#include "Core/Component.h"
#include "Core/Camera.h"
#include "Core/Game.h"
#include <iostream>

Text::Text(
    GameObject &associated,
    std::string fontFile,
    int fontSize,
    TextStyle style,
    std::string text,
    SDL_Color color,
    int blink,
    bool cameraFollower) : Component(associated), blink(blink)
{
    this->font = Resources::GetFont(fontFile, fontSize);
    this->texture = nullptr;
    this->text = text;
    this->style = style;
    this->fontFile = fontFile;
    this->color = color;
    this->fontSize = fontSize;
    this->blink = Timer(blink);
    this->blink.Restart();
    this->cameraFollower = cameraFollower;
    appear = true;
    RemakeTexture();
}
Text::~Text()
{
    if (texture != nullptr)
    {
        GPU_FreeImage(texture);
        texture = nullptr;
    }
}
void Text::Update(float dt)
{
    if (blink.GetAmount() > 0)
    {
        blink.Update(dt);
        if (blink.Expired())
        {
            blink.Restart();
            appear = !appear;
        }
    }
}
void Text::Render()
{
    if (appear)
    {
        GPU_Rect clipRect = {0, 0, (float)associated.box.GetSize().x, (float)associated.box.GetSize().y};
        Vec2 pos = associated.box.GetPos();
        GPU_Rect dstRect = {(float)pos.x*Camera::zoom, (float)pos.y*Camera::zoom, (float)associated.box.GetSize().x*Camera::zoom, (float)associated.box.GetSize().y*Camera::zoom};
        if (!cameraFollower)
        {
            dstRect.y -= Camera::pos.y;
            dstRect.x -= Camera::pos.x;
        }
        GPU_BlitRectX(texture, &clipRect, Game::GetInstance().GetGPUTarget(), &dstRect, this->associated.angleDeg, texture->w / 2, texture->h / 2, SDL_FLIP_NONE);
    }
}
bool Text::Is(std::string type)
{
    return type == "Text";
}
void Text::SetText(std::string text)
{
    this->text = text;
    RemakeTexture();
}
void Text::SetColor(SDL_Color color)
{
    this->color = color;
    RemakeTexture();
}
void Text::SetStyle(TextStyle style)
{
    this->style = style;
    RemakeTexture();
}
void Text::SetFontFile(std::string fontFile)
{
    this->fontFile = fontFile;
    RemakeTexture();
}
void Text::SetFontSize(int fontSize)
{
    this->fontSize = fontSize;
    RemakeTexture();
}
void Text::RemakeTexture()
{
    if (this->texture != nullptr)
    {
        GPU_FreeImage(this->texture);
        this->texture = nullptr;
    }
    SDL_Surface *surface;
    switch (style)
    {
    case SOLID:
        surface = TTF_RenderText_Solid(this->font, this->text.c_str(), this->color);
        break;
    case SHADED:
        surface = TTF_RenderText_Shaded(this->font, this->text.c_str(), this->color, {0, 0, 0});
        break;
    case BLENDED:
        surface = TTF_RenderText_Blended(this->font, this->text.c_str(), this->color);
        break;
    }
    this->texture = GPU_CopyImageFromSurface(surface);
    GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
    if (this->texture != NULL)
    {
        this->associated.box.SetSize({surface->w, surface->h});
    }
    SDL_FreeSurface(surface);
}