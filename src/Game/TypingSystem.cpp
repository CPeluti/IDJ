#include "Game/TypingSystem.h"
#include "Game/Character.h"
#include "Core/Log.h"

TypingSystem::TypingSystem(Text *textComponent, bool enableCursor, bool isTypingMode) : textComponent(textComponent),
                                                                                        enableCursor(enableCursor),
                                                                                        isTypingMode(isTypingMode),
                                                                                        cursorVisible(false),
                                                                                        submitted(false),
                                                                                        cursorTimer(0.0f),
                                                                                        text(" ")
{
}

void TypingSystem::HandleInput(const SDL_Event &event)
{
  switch (event.type)
  {
  case SDL_TEXTINPUT:
  {
    this->text += event.text.text;
  }
  break;
  case SDL_KEYDOWN:
  {
    if (event.key.keysym.sym == SDLK_BACKSPACE && !this->text.empty())
    {
      this->text.pop_back();
    }
    else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
    {
      this->submitted = true;
      this->HandleSubmit();
    }
  }
  default:
    break;
  }
  this->UpdateTextWithCursor();
}

void TypingSystem::HandleSubmit()
{
  if (this->submitted)
  {
    this->CleanText();
    this->ResetSubmission();
  }
}

void TypingSystem::Update(float dt)
{
  if(auto player = Character::player.lock()){
    auto pBox = player->getAssociated()->box;
    this->textComponent->getAssociated()->box.Move(pBox.center()-Vec2{.0,pBox.GetSize().x/2});

    if (!this->enableCursor || !this->isTypingMode)
    return;
    
    cursorTimer += dt;
    if (cursorTimer >= cursorBlinkInterval)
    {
      cursorVisible = !cursorVisible;
      cursorTimer = 0.0f;
      this->UpdateTextWithCursor();
    }
  }
}

void TypingSystem::UpdateTextWithCursor()
{
  if (this->textComponent)
  {
    if (this->enableCursor && this->cursorVisible)
    {
      this->textComponent->SetText(this->text + "|");
    }
    else
    {
      this->textComponent->SetText(this->text);
    }
  }
}