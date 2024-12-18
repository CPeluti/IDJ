#include "Animator.h"
#include "SpriteRenderer.h"

Animator::Animator(GameObject &associated) : Component(associated), frameStart(0), frameEnd(0), frameTime(0), currentFrame(0), timeElapsed(0) {}
void Animator::Update(float dt) {
    int oldCurrentFrame = currentFrame;
    if(frameTime != 0) timeElapsed++;
    if(timeElapsed > frameTime){
        currentFrame++;
        timeElapsed -= frameTime;
    }
    if(currentFrame > frameEnd){
        currentFrame = frameStart;
    }
    if(currentFrame != oldCurrentFrame){
        ((SpriteRenderer*)this->associated.GetComponent("SpriteRenderer"))->SetFrame(currentFrame);
    }
}
void Animator::Render() {}
bool Animator::Is(std::string type) {
    return type == "Animator";
}
void Animator::SetAnimation(std::string name) {
    if(animations.find(name) != animations.end()) {
        Animation *animation = animations[name];
        frameStart = animation->frameStart;
        frameEnd = animation->frameEnd;
        frameTime = animation->frameTime;
        currentFrame = frameStart;
        timeElapsed = 0;
        ((SpriteRenderer*)this->associated.GetComponent("SpriteRenderer"))->SetFrame(currentFrame);
    }
}
void Animator::AddAnimation(std::string name, Animation *animation) {
    if(animations.find(name) == animations.end()) {
        animations[name] = animation;
    }
}