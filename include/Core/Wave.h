#include "Timer.h"
class Wave {
    public:
        Wave(int zombies, int npcs, float cooldown);
        int zombies;
        int npcs;
        Timer cooldown;
};