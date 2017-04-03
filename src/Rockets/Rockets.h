#pragma once

#include "vector"
#include "cinder\Vector.h"

namespace Rockets {

  struct Object {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    glm::float1 mass = 1.0f;
  };

  struct Sphere {
    Object object = Object();
    glm::float1 radius;
  };

  struct Rectangle {
    Object object = Object();
    glm::vec3 bounds = glm::vec3(0);
  };

  struct Endpoint {
    glm::vec3 position = glm::vec3(0);
  };

  struct FeulCell {
    glm::vec3 booster1 = glm::vec3(0);
    glm::vec3 booster2 = glm::vec3(0);
    glm::vec3 booster3 = glm::vec3(0);

    float length() {
      return glm::length(booster1) + glm::length(booster2) + glm::length(booster3);
    }
  };

  struct Rocket {
    Sphere object = Sphere();
    std::vector<FeulCell> feul;
    int feulUsed = 0;
  };

  struct World {
    Rectangle ground = Rectangle();
    Sphere endpoint = Sphere();
    Rocket rocket = Rocket();
    glm::float1 worldTime = 0;
  };

  struct SimulationOptions {
    glm::float1 timeStep = 1.0f / 60.0f; // 60 times per second
    glm::vec3 gravity = glm::vec3(0, -9.81, 0);
  };

  World Step(const SimulationOptions& options, const World& world);
  void StepInPlace(const SimulationOptions& options, World &world);

}