#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject& associated){
    this->associated = associated;
}