#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(GameObject& associated) : Component(associated) {
    this->sprite = Sprite();
    this->frameStart = 0;
    this->frameEnd = 0;
    this->frameTime = 0;
    this->currentFrame = 0;
    this->timeElapsed = 0;
}

SpriteRenderer::SpriteRenderer(GameObject& associated, std::string file, int frameCountW, int frameCountH) : Component(associated) {
    this->sprite = Sprite(file, frameCountW, frameCountH);
    associated.box.w = sprite.GetWidth();
    associated.box.h = sprite.GetHeight();
    this->frameStart = 0;
    this->frameEnd = 0;
    this->frameTime = 0;
    this->currentFrame = 0;
    this->timeElapsed = 0;
}

void SpriteRenderer::SetFrameCount(int frameCountW, int frameCountH) {
    this->sprite.SetFrameCount(frameCountW, frameCountH);
}

void SpriteRenderer::Open(std::string file) {
    this->sprite.Open(file);
}

void SpriteRenderer::Update(float dt) {
    int oldCurrentFrame = currentFrame;
    if(this->frameTime > 0) {
        timeElapsed++;
    }
    if(timeElapsed > frameTime) {
        currentFrame++;
        frameTime -= timeElapsed;
    }
    if(currentFrame > frameEnd) {
        currentFrame = frameStart;
    }
    if(oldCurrentFrame != currentFrame) {
        sprite.SetFrame(currentFrame);
    }
}

void SpriteRenderer::Render() {
    sprite.Render(associated.box.x, associated.box.y, associated.box.w, associated.box.h);
}

void SpriteRenderer::SetAnimation(Animation* animation) {
    this->frameStart = animation->frameStart;
    this->frameEnd = animation->frameEnd;
    this->frameTime = animation->frameTime;
    this->currentFrame = frameStart;
    this->timeElapsed = 0;
    sprite.SetFrame(currentFrame);
}