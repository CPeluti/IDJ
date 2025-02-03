#include "Timer.h"
class Wave {
    public:
        Wave(int zombies, float cooldown);
        int zombies;
        Timer cooldown;
};