#pragma once

#include "vector"
#include "cinder\Vector.h"

namespace Rockets {

  struct Object {
    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::quat();
    glm::vec3 momentum = glm::vec3(0);
    glm::vec3 angularMomentum = glm::vec3(0);

    glm::float1 mass = 1.0f;
    glm::mat3 inertiaTensorBody = glm::mat3(0);
    glm::mat3 invertedInertiaTensorBody = glm::mat3(0);

    auto velocity() const {
      return (1.0f / mass) * momentum;
    };

    auto rotationMatrix() const {
      return glm::toMat3(rotation);
    }

    auto inertiaTensor() const {
      auto r = rotationMatrix();
      return r * inertiaTensorBody * glm::transpose(r);
    };

    auto inertiaTensorInverse() const {
      auto r = rotationMatrix();
      return r * invertedInertiaTensorBody * glm::transpose(r);
    };

    auto angularVelocity() const {
      return inertiaTensorInverse() * angularMomentum;
    };
  };

  struct Sphere : public Object {
    glm::float1 radius;
  };

  struct Rectangle : public Object {
    glm::vec3 bounds = glm::vec3(1);

    auto calculateInertiaTensorBody() const {
      return (mass / 12.0f) * glm::mat3(glm::pow(bounds.y,2) + glm::pow(bounds.z,2),0,0,
                                        0, glm::pow(bounds.x, 2) + glm::pow(bounds.z, 2),0,
                                        0,0, glm::pow(bounds.x, 2) + glm::pow(bounds.y, 2));
    }
  };

  struct Endpoint {
    glm::vec3 position = glm::vec3(0);
  };

  struct FeulCell {
    glm::vec4 boosters;

    auto torque(glm::mat3 r) const {
      auto down = glm::vec3(1, -1.5, 1) * r;
      auto du = glm::vec3(0, boosters[0], 0) * r;

      auto down2 = glm::vec3(-1, -1.5, 1) * r;
      auto du2 = glm::vec3(0, boosters[1], 0)* r;

      auto down3 = glm::vec3(-1, -1.5, -1) * r;
      auto du3 = glm::vec3(0, boosters[2], 0) * r;

      auto down4 = glm::vec3(1, -1.5, -1) * r;
      auto du4 = glm::vec3(0, boosters[3], 0) * r;

      return glm::cross(down, du) + glm::cross(down2, du2) + glm::cross(down3, du3) + glm::cross(down4, du4);
    }
    auto force(glm::mat3 r) const {
      return r * glm::vec3(0.0f, boosters[0] + boosters[1] + boosters[2] + boosters[3], 0.0f);
    }
  };

  struct Rocket : public Rectangle {
    std::vector<FeulCell> feul;
    int feulUsed = 0;
  };

  struct World {
    Rectangle ground = Rectangle();
    Sphere endpoint = Sphere();
    Rocket rocket = Rocket();
    glm::float1 worldTime = 0;
    glm::int64 steps = 0;
  };

  struct SimulationOptions {
    glm::float1 timeStep = 1.0f / 60.0f; // 60 times per second
    glm::float1 frictionCoefficient = -0.35;
    glm::vec3 gravity = glm::vec3(0, -9.81, 0);
  };

  World Step(const SimulationOptions& options, const World& world);
  void StepInPlace(const SimulationOptions& options, World &world);
}