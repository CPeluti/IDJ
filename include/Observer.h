#pragma once
#include <vector>
#include <Component.h>
class Subject;
class Observer {
    friend class Subject;
    public:
        Observer();
        enum Event { onTakeDamage };
        virtual ~Observer();
        virtual void onNotify(const Component& component,Event event) = 0;
    private:
        std::vector<Subject*> subjects;
        Observer* next;
};