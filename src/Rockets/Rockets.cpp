
#include "Rockets.h"

namespace Rockets {

  auto star(glm::vec3 a) {
    return glm::mat3(0,  -a.z, a.y,
                     a.z, 0,  -a.x,
                    -a.y, a.x, 0);
  }

  auto changeBasis(glm::vec3 a, glm::vec3 b) {
    auto c = glm::dot(b, a);
    auto v = glm::cross(b, a);
    auto cob = glm::mat3(1) + star(v) + star(v)*star(v)*(1 / (1 + c));
    return cob;
  }

  auto calculateFriction(glm::vec3 momentum, glm::float1 frictionCoefficient)
  {
    auto scaledFriction = glm::length(momentum)*frictionCoefficient;
    if (glm::abs(scaledFriction) < glm::abs(frictionCoefficient))
    {
      scaledFriction = frictionCoefficient;
    }

    auto friction = scaledFriction * glm::normalize(momentum);
    if (glm::dot(friction, momentum) < 0) return friction;

    return glm::vec3(0);
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
    world.steps++;

    auto booster = glm::vec3(0);
    if (world.rocket.feul.size() > world.rocket.feulUsed) {
      booster = world.rocket.feul[world.rocket.feulUsed].boosters();
      if (world.rocket.feulUsed < world.rocket.feul.size() && (world.steps % (int)ceil(powf(options.timeStep, -1))) == 0)
      {
        world.rocket.feulUsed++;
      }
    }

    auto acceleration = booster + (options.gravity * world.rocket.mass);
    world.rocket.momentum += (acceleration * options.timeStep);
    world.rocket.momentum += calculateFriction(world.rocket.momentum, options.frictionCoefficient) * options.timeStep;
    world.rocket.position += (world.rocket.velocity() * options.timeStep);
    world.rocket.rotation = glm::normalize(0.5f * glm::quat(world.rocket.angularVelocity() * options.timeStep) * world.rocket.rotation);
  }
}