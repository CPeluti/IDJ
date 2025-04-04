#include <vector>

#include "Core/Component.h"
#include "Core/Timer.h"

#include "Game/Wave.h"
class WaveSpawner : public Component
{
public:
    WaveSpawner(GameObject &associated);
    void Update(float dt);
    void Render();
    bool Is(std::string type);

private:
    int zombieCounter;
    int npcCounter;
    std::vector<Wave *> waves;
    int currentWave;
};