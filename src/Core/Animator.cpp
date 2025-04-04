#include "Core/Animator.h"
#include "Core/SpriteRenderer.h"
#include <iostream>

Animator::Animator(GameObject &associated) : Component(associated), current(""), frameStart(0), frameEnd(0), frameTime(0), currentFrame(0), timeElapsed(0) {}
void Animator::Update(float dt) {
    int oldCurrentFrame = currentFrame;
    if(frameTime != 0) timeElapsed+= dt;
    if(timeElapsed > frameTime){
        currentFrame++;
        timeElapsed -= frameTime;
    }
    if(currentFrame > frameEnd){
        currentFrame = frameStart;
    }
    if(currentFrame != oldCurrentFrame){
        Animation* a = animations[current];
        ((SpriteRenderer*)this->associated.GetComponent("SpriteRenderer"))->SetFrame(currentFrame, a->flip);
    }
}
void Animator::Render() {}
void Animator::SetAnimation(std::string name) {
    if(animations.find(name) != animations.end() && name != current) {
        current = name;
        Animation *animation = animations[name];
        frameStart = animation->frameStart;
        frameEnd = animation->frameEnd;
        frameTime = animation->frameTime;
        currentFrame = frameStart;
        timeElapsed = 0;
        SpriteRenderer* sr = ((SpriteRenderer*)this->associated.GetComponent("SpriteRenderer"));
        if(sr){
            sr->SetFrame(currentFrame, animation->flip);
        }
    }
}
void Animator::AddAnimation(std::string name, Animation *animation) {
    if(animations.find(name) == animations.end()) {
        animations[name] = animation;
    }
}