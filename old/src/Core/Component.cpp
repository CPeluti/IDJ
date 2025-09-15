#include "Core/Component.h"
#include "Core/GameObject.h"

Component::Component(GameObject &associated) : associated(associated) {}
Component::~Component() {}

void Component::Start(){}
// void Component::On(GameObject& other){}

double Component::getAngleDeg(){
    return associated.angleDeg;
}
void Component::notifyEvent(Event& evt){
    associated.subject.notify(evt);
}