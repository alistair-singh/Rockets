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
    glm::vec3 booster1 = glm::vec3(0);
    glm::vec3 booster2 = glm::vec3(0);
    glm::vec3 booster3 = glm::vec3(0);

    auto length() const {
      return glm::length(booster1) + glm::length(booster2) + glm::length(booster3);
    }

    auto boosters() const {
      return (booster1 + booster2 + booster3);
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