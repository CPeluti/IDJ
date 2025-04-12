#include "Core/Component.h"
#include "Core/GameObject.h"

Component::Component(GameObject &associated) : associated(associated) {}
Component::~Component() {}

void Component::Start(){}
// void Component::On(GameObject& other){}
