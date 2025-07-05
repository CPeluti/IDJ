#include "Core/Subject.h"
#include <cstddef>

Subject::Subject():head(NULL){}

void Subject::addObserver(Observer* observer){
    observer->subjects.push_back(this);
    observer->next = head;
    head = observer;
}

void Subject::removeObserver(Observer* observer){
    if(head == observer){
        head=observer->next;
        observer->next = NULL;
        return;
    }

    Observer* current = head;

    while(current){
        if(current->next == observer){
            current->next = observer->next;
            observer->next = NULL;
            return;
        }
        current = current->next;
    }
}

void Subject::notify(Event& evt){
    Observer* observer = head;
    while(observer != NULL){
        observer->OnEvent(evt);
        observer = observer->next;
    }
}