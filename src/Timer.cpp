#include "Timer.h"

Timer::Timer(float amount):amount(amount),time(0){}

void Timer::Update(float dt){
    time += dt;
}

void Timer::Restart(){
    time = 0;
}

bool Timer::Expired(){
    return time > amount;
}
