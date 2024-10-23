#include "AnimationSetter.h"
#include "SpriteRenderer.h"

AnimationSetter::AnimationSetter(GameObject &associated) : Component(associated) {}
void AnimationSetter::Update(float dt) {}
void AnimationSetter::Render() {}
bool AnimationSetter::Is(std::string type) {
    return type == "AnimationSetter";
}
void AnimationSetter::SetAnimation(std::string name) {
    if(animations.find(name) != animations.end()) {
        Animation *animation = animations[name];
        SpriteRenderer* sr = (SpriteRenderer*)associated.GetComponent("SpriteRenderer") ;
        sr->SetAnimation(animation);
    }
}
void AnimationSetter::AddAnimation(std::string name, Animation *animation) {
    if(animations.find(name) == animations.end()) {
        animations[name] = animation;
    }
}