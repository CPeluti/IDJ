#pragma once
#include "Observer.h"
#include "Component.h"
class Subject {
    public:
        Subject();
        void notify(const Component& e, Observer::Event ev);
        void addObserver(Observer* observer);
        void removeObserver(Observer* observer);
    private:
        Observer* head;
};