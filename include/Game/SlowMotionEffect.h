#include "Core/Timer.h"
#include "Effect.h"
#include "Entity.h"
#include "Core/Log.h"
#include <iostream>

class SlowMotionEffect : public Effect
{
private:
  float speedReduction;
  Timer durationTimer;

public:
  SlowMotionEffect(float speedReduction, float duration) : durationTimer(duration) { this->speedReduction = speedReduction; };

  void Apply(Entity &entity, float dt)
  {
    entity.SetMovementSpeed(entity.GetMovementSpeed() - speedReduction);
    durationTimer.Update(dt);
  }

  bool IsExpired()
  {
    return durationTimer.Expired();
  }
};