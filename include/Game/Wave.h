#include "Core/Timer.h"
class Wave {
    public:
        Wave(int enemies, float cooldown);
        int enemies;
        Timer cooldown;
};