
#include "Rockets.h"

namespace Rockets {

  auto star(glm::vec3 a) {
    return glm::mat3(0,  -a.z, a.y,
                     a.z, 0,  -a.x,
                    -a.y, a.x, 0);
  }

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
      booster = world.rocket.feul[world.rocket.feulUsed].boosters();

      if ((((int)(world.worldTime / options.timeStep)) % (60 * 5)) == 0)
      {
        world.rocket.feulUsed++;
      }
    }
    auto acceleration = booster + (options.gravity * world.rocket.mass);
    world.rocket.velocity += ((acceleration / world.rocket.mass) * options.timeStep);
    
    auto f1 = glm::length(world.rocket.velocity)*options.frictionCoefficient;
    if (glm::abs(f1) < glm::abs(options.frictionCoefficient))
    {
      f1 = options.frictionCoefficient;
    }
    auto friction = (f1 * options.timeStep) * glm::normalize(world.rocket.velocity);
    auto s = glm::dot(friction, world.rocket.velocity);
    if (s < 0)
    {
      world.rocket.velocity += friction;
    }
    else
    {
      world.rocket.velocity = glm::vec3(0);
    }
    world.rocket.position += (world.rocket.velocity * options.timeStep);

    auto sta = star(world.rocket.angularVelocity);
    auto rot =  sta * world.rocket.rotation * options.timeStep;
    auto rot2 = world.rocket.rotation + rot;
    world.rocket.rotation = rot2;
  }
}