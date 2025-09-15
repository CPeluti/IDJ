#include "Game/TypingSystem.h"
#include "Game/Character.h"
#include "Core/InputManager.h"
#include "Core/Log.h"

std::vector<std::string> split(const std::string str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

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
   InputManager ip = InputManager::GetInstance();

  switch (event.type)
  {
  case SDL_TEXTINPUT:
  {
    this->text += event.text.text;
  }
  break;
  case SDL_KEYDOWN:
  {
    if (event.key.keysym.sym == SDLK_BACKSPACE && !this->text.empty() && this->text != " ")
    {
      this->text.pop_back();
      if (this->text.empty()) {
          this->text.clear();
      }
    }
    else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
    {
      this->SetIsTypingMode(true);
      this->SetSubmitted(true);
      this->HandleSubmit();
      Vec2 target;
      target.x = ip.GetMouseX();
      target.y = ip.GetMouseY();
      Character::Command c = Character::Command(Character::Command::SHOOT, target);
      if (auto character = std::dynamic_pointer_cast<Character>(Character::player.lock())) {
          character->Issue(c);
      }
    }
    else if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
		this->isTypingMode = false;
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
	this->submittedText = split(this->text);
    this->CleanText();
    this->ResetSubmission();
  }
}

void TypingSystem::Update(float dt)
{
  if(auto player = Character::player.lock()){
    auto pBox = player->getAssociated()->box;
    this->textComponent->getAssociated()->box.Move((pBox.center() - (Vec2{ .0,pBox.GetSize().x / 2 }*Camera::zoom)));

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