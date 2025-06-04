#pragma once
#include "Core/Text.h"
#include "Core/Game.h"

class TypingSystem
{
public:
  static TypingSystem &GetInstance()
  {
    static TypingSystem typingSystem(nullptr, true, false);
    return typingSystem;
  }

  void Update(float dt);
  void HandleInput(const SDL_Event &event);
  void HandleSubmit();

  void CleanText()
  {
    this->text = " ";
  };

  std::string GetText()
  {
    return this->text;
  };

  bool HasSubmitted()
  {
    return this->submitted;
  };

  void ResetSubmission()
  {
    this->submitted = false;
  };

  void SwitchTypingMode()
  {
    if (!this->isTypingMode)
    {
      this->cursorVisible = false;
      this->UpdateTextWithCursor();
    }
    this->isTypingMode = !this->isTypingMode;
  };
  bool IsTypingMode()
  {
    return this->isTypingMode;
  };

  void SetTextComponent(Text *textComponent)
  {
    this->textComponent = textComponent;
  };

private:
  TypingSystem(Text *textComponent, bool enableCursor = true, bool isTypingMode = true);

  Text *textComponent;
  std::string text;
  bool isTypingMode;
  bool enableCursor;
  bool cursorVisible;
  float cursorTimer;
  bool submitted;
  const float cursorBlinkInterval = 0.5f;

  void UpdateTextWithCursor();
};