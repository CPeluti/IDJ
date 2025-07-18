#include "Core/Component.h"
#include "Game/Wave.h"
#include "Core/Timer.h"
#include <vector>
class WaveSpawner : public Component
{
public:
    WaveSpawner(GameObject &associated);
    void Update(float dt);
    void Render();
    bool Is(std::string type);
    void Start();

private:
    int enemyCounter;
	bool bossSpawned;
    int bossCounter;
    std::vector<Wave *> waves;
    int currentWave;
};