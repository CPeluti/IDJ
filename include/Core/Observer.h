#pragma once
#include <vector>
#include "Component.h"
class Subject;
class Observer {
    friend class Subject;
    public:
        Observer();
        virtual ~Observer();
        virtual void OnEvent(Event& e) = 0;
    private:
        std::vector<Subject*> subjects;
        Observer* next;
};