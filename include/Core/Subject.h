#pragma once
#include "Observer.h"
#include "Component.h"
class Subject {
    public:
        Subject();
        void notify(Event& evt);
        void addObserver(Observer* observer);
        void removeObserver(Observer* observer);
    private:
        Observer* head;
};