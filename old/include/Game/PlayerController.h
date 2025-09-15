#pragma once
#include "Core/GameObject.h"
#include <string>

class PlayerController : public Component{
    public:
        PlayerController (GameObject& associated);
        void Start();
        void Update (float dt);
        void Render ();
        bool Is (std::string type);
};