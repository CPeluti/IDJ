#include "Core/Subject.h"
#include "Core/Observer.h"
Observer::Observer():next(NULL){}
Observer::~Observer(){
    for(int i = 0; i<subjects.size(); i++){
        subjects[i]->removeObserver(this);
    }
}