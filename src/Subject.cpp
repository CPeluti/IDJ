#include "Subject.h"
#include "Lifebar.h"
#include <cstddef>

Subject::Subject():head(NULL){}

void Subject::addObserver(Observer* observer){
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

    while(current!=NULL){
        if(current->next == observer){
            current->next = observer->next;
            observer->next = NULL;
            return;
        }
        current = current->next;
    }
}

void Subject::notify(const Component& component, Observer::Event event){
    Observer* observer = head;
    while(observer != NULL){
        observer->onNotify(component, event);
        observer = observer->next;
    }
}