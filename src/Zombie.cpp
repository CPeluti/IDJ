#include "Zombie.h"

Zombie::Zombie(GameObject &associated) : Component(associated), hitpoints(100)
{
}