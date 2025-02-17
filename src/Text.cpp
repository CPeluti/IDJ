#include "Text.h"
#include "Resources.h"
#include "Component.h"
#include "Camera.h"
#include "Game.h"

Text::Text (
    GameObject& associated,
    std::string fontFile, 
    int fontSize ,
    TextStyle style ,
    std::string text ,
    SDL_Color color,
    int blink
): Component(associated), blink(blink)
{
    this->font=Resources::GetFont(fontFile, fontSize);
    this->texture=nullptr;
    this->text=text;
    this->style=style;
    this->fontFile=fontFile;
    this->color=color;
    this->fontSize=fontSize;
    this->blink = Timer(blink);
    this->blink.Restart();
    appear = true;
    RemakeTexture();
}
Text::~Text(){
    if(texture != nullptr){
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}
void Text::Update(float dt){
    if(blink.GetAmount()>0){
        blink.Update(dt);
        if(blink.Expired()){
            blink.Restart();
            appear = !appear;
        }
    }
}
void Text::Render(){
    if(appear){
        SDL_Rect clipRect = {0,0,(int)associated.box.GetSize().x, (int)associated.box.GetSize().y};
        Vec2 pos = associated.box.GetPos() - Camera::pos;
        SDL_Rect dstRect = {(int)pos.x, (int)pos.y, (int)associated.box.GetSize().x, (int)associated.box.GetSize().y};
        SDL_RenderCopyEx(Game::GetInstance().GetRenderer(), texture, &clipRect, &dstRect, this->associated.angleDeg, nullptr, SDL_FLIP_NONE);
    }
}
bool Text::Is(std::string type){
    return type == "Text";
}
void Text::SetText(std::string text){
    this->text = text;
    RemakeTexture();
}
void Text::SetColor(SDL_Color color){
    this->color = color;
    RemakeTexture();
}
void Text::SetStyle(TextStyle style){
    this->style = style;
    RemakeTexture();
}
void Text::SetFontFile(std::string fontFile){
    this->fontFile = fontFile;
    RemakeTexture();
}
void Text::SetFontSize(int fontSize){
    this->fontSize = fontSize;
    RemakeTexture();
}
void Text::RemakeTexture(){
    if(texture != nullptr){
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    SDL_Surface* surface;
    switch (style)
    {
        case SOLID:
            surface = TTF_RenderText_Solid(font, text.c_str(), color);
            break;
        case SHADED:
            surface = TTF_RenderText_Shaded(font, text.c_str(), color, {0,0,0});
            break;
        case BLENDED:
            surface = TTF_RenderText_Blended(font, text.c_str(), color);
            break;
    }
    texture = SDL_CreateTextureFromSurface(Game::GetInstance().GetRenderer(), surface);
    if(texture != NULL){
        associated.box.SetSize({surface->w, surface->h});
    }
    SDL_FreeSurface(surface);
}