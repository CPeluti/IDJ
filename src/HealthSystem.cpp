#include "Core/HealthSystem.h"

float HealthSystem::TakeDamage(float damage){
    hp -= damage;
    return damage;
}