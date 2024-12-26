#include "Timer.h"

Timer::Timer(float amount):amount(amount),time(amount){}

void Timer::Update(float dt){
    time += dt;
}

void Timer::Restart(){
    time = 0;
}

bool Timer::Expired(){
    return time >= amount;
}
