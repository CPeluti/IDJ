#include "Core/State.h"

State::State() : popRequested(false), started(false), quitRequested(false), objectArray() {}
State::~State()
{
    this->objectArray.clear();
}

std::weak_ptr<GameObject> State::AddObject(GameObject *object)
{
    objectArray.emplace_back(object);
    if (started)
    {
        object->Start();
    }
    return GetObjectPtr(object);
}

std::weak_ptr<GameObject> State::GetObjectPtr(GameObject *object)
{
    for (unsigned int i = 0; i < objectArray.size(); i++)
    {
        if (objectArray[i].get() == object)
        {
            return std::weak_ptr<GameObject>(objectArray[i]);
        }
    }
    return std::weak_ptr<GameObject>();
}

bool State::PopRequested()
{
    return popRequested;
}
bool State::QuitRequested()
{
    return quitRequested;
}
void State::StartArray()
{
    for (int i = 0; i < (int)objectArray.size(); i++)
    {
        objectArray[i]->Start();
    }
}
void State::UpdateArray(float dt)
{
    for (int i = 0; i < (int)objectArray.size(); i++)
    {
        objectArray[i]->Update(dt);
    }
}
void State::RenderArray()
{
    for (int i = 0; i < (int)objectArray.size(); i++)
    {
        objectArray[i]->Render();
    }
}