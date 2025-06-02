#pragma once

// #define COMPONENT_IS(targetType) inline bool Is(std::string type) const override { return type == targetType;}

#include <string>
#include "Events/BasicEvent.h"
// #include "GameObject.h"

class GameObject;
class Component
{
public:
    Component(GameObject &associated);
    virtual ~Component();
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Start();
    virtual bool Is(std::string type) = 0;
    virtual void OnEvent (Event& evt) {};
    inline GameObject* getAssociated(){
        return &associated;
    }

    
    double getAngleDeg();
    void notifyEvent(Event& evt);
    // COMPONENT_IS("Component");

protected:
    GameObject &associated;
};
