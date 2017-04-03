
#include "Rockets.h"

namespace Rockets {

  World Step(const SimulationOptions& options, const World& world)
  {
    auto newWorld = world;
    StepInPlace(options, newWorld);
    return newWorld;
  }

  void StepInPlace(const SimulationOptions& options, World &world)
  {
    world.worldTime += options.timeStep;
    auto booster = glm::vec3(0);

    if (world.rocket.feul.size() > world.rocket.feulUsed) {
      booster = (world.rocket.feul[world.rocket.feulUsed].booster1
        + world.rocket.feul[world.rocket.feulUsed].booster2
        + world.rocket.feul[world.rocket.feulUsed].booster3);

      if ((((int)(world.worldTime / options.timeStep)) % (60 * 5)) == 0)
      {
        world.rocket.feulUsed++;
      }
    }
    auto acceleration = booster + (options.gravity * world.rocket.object.object.mass);
    world.rocket.object.object.velocity += ((acceleration / world.rocket.object.object.mass) * options.timeStep);
    world.rocket.object.object.position += (world.rocket.object.object.velocity * options.timeStep);
  }
}