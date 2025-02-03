#include "Component.h"
#include "Wave.h"
#include "Timer.h"
#include <vector>
class WaveSpawner : public Component
{
public:
    WaveSpawner(GameObject &associated);
    void Update(float dt);
    void Render();
    bool Is(std::string type);

private:
    int zombieCounter;
    std::vector<Wave *> waves;
    int currentWave;
};