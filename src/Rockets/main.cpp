#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Rockets.h"

//#include <Windows.h>
//
//
//// For debugging
//#include <io.h>
//#include <fcntl.h>

using namespace ci;
using namespace ci::app;
using namespace std;

auto makeFeulCells(int amount)
{
  std::vector<Rockets::FeulCell> result;
  if (amount < 0) return result;

  std::generate_n(std::back_inserter(result), amount, [] {
    Rockets::FeulCell cell;
    auto random = [] { return static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX); };
    //cell.booster1 = vec3(random(), random() + 0.0981, random()) * 10.0f;
    cell.booster1 = vec3(0, 9.81 + 0.35, 0);
    /*
    cell.booster2 = vec3(random(), random(), random()) * 10.0f;
    cell.booster3 = vec3(random(), random(), random()) * 10.0f;
    */
    return cell;
  });

  return result;
}

Rockets::World InitialWorld() {
  Rockets::World world;
  world.rocket.bounds = vec3(1, 3, 1);
  world.rocket.position = vec3(0, 1.5, 0);
  world.rocket.mass = 1.0f;
  world.rocket.angularVelocity = vec3(0,10,0);
  world.rocket.feul = makeFeulCells(1000);
  return world;
}

class RocketsApp : public App {
public:
  void setup() override {
    //if (AllocConsole()) {
    //  FILE* pCout;
    //  freopen_s(&pCout, "CONOUT$", "w", stdout);
    //  SetConsoleTitle(L"Debug Console");
    //  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    //}

    // Set up the camera.
    mCamera.lookAt(cameraPosition, vec3(0, 20.0f, 0));
    mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 100000);
    mCamUi = CameraUi(&mCamera, getWindow());

    // Create batches that render fast.
    auto colorShader = gl::getStockShader(gl::ShaderDef().color());
    auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());

    mFloor = gl::Batch::create(geom::Plane().size(vec2(30)).subdivisions(ivec2(30)), colorShader);
    mRocket = gl::Batch::create(geom::Cube().size(1, 1, 1), lambertShader);
    mEndpoint = gl::Batch::create(geom::Sphere(), lambertShader);

    mWorld = InitialWorld();
  }

  void update() override {
    auto currentTime = getElapsedSeconds();
    while ((currentTime - elapseTime) > mOptions.timeStep) {
      Rockets::StepInPlace(mOptions, mWorld);
      elapseTime += mOptions.timeStep;
    }
    if (follow) {
      mCamera.lookAt(mWorld.rocket.position);
    }
  }

  void draw() override {
    // Gray background.
    gl::clear(Color::hex(0x251838));

    // Set up the camera.
    gl::ScopedMatrices push;
    gl::setMatrices(mCamera);

    // Enable depth buffer.
    gl::ScopedDepth depth(true);

    // Draw the centre.
    //{
    //  gl::ScopedColor color(Color::hex(0x5492f7));
    //  gl::ScopedModelMatrix model;

    //  gl::multModelMatrix(glm::scale(vec3(0.5f)));
    //  mEndpoint->draw();
    //}

    // Draw Axis.
    {
      gl::ScopedModelMatrix model;
      auto origin = vec3(3, 3, 3);
      gl::ScopedColor c1(Color::hex(0xff0000));
      gl::drawVector(origin, origin + vec3(1, 0, 0));

      gl::ScopedColor c2(Color::hex(0x00ff00));
      gl::drawVector(origin, origin + vec3(0, 1, 0));

      gl::ScopedColor c3(Color::hex(0x0000ff));
      gl::drawVector(origin, origin + vec3(0, 0, 1));
    }

    // Draw the end.
    {
      gl::ScopedColor color(Color::hex(0x5492f7));
      gl::ScopedModelMatrix model;

      gl::multModelMatrix(glm::translate(vec3(0, 35.0f, 0)));
      mEndpoint->draw();
    }

    // Draw the rocket.
    {
      gl::ScopedColor color(Color::hex(0xf44242));
      gl::ScopedModelMatrix model;

      auto m1 = mat4(mWorld.rocket.rotation);
      auto m2 = glm::translate(mWorld.rocket.position);
      auto m3 = glm::scale(mWorld.rocket.bounds);
      gl::multModelMatrix(m2 * m1 * m3);
      mRocket->draw();
    }

    // Draw the floor.
    {
      gl::ScopedColor color(Color::hex(0x183818));
      mFloor->draw();
    }

    //std::cout << "[" << mWorld.rocket.rotation[0].x << ", " << mWorld.rocket.rotation[0].y << ", " << mWorld.rocket.rotation[0].z << "\n"
    //                 << mWorld.rocket.rotation[1].x << ", " << mWorld.rocket.rotation[1].y << ", " << mWorld.rocket.rotation[1].z << "\n"
    //                 << mWorld.rocket.rotation[2].x << ", " << mWorld.rocket.rotation[2].y << ", " << mWorld.rocket.rotation[2].z << "]\n";
  }

  void keyDown(KeyEvent ev) override {
    switch (ev.getCode())
    {
    case KeyEvent::KEY_SPACE:
      follow = false;
      cameraPosition = vec3(30, 44, 30);
      mCamera.lookAt(cameraPosition, vec3(0, 20.0f, 0));
      break;
    case KeyEvent::KEY_f:
      follow = !follow;
      break;
    case KeyEvent::KEY_w:
    case KeyEvent::KEY_UP:
      cameraPosition.y += 1;
      break;
    case KeyEvent::KEY_s:
    case KeyEvent::KEY_DOWN:
      cameraPosition.y -= 1;
      break;
    case KeyEvent::KEY_a:
    case KeyEvent::KEY_LEFT:
      cameraPosition.x -= 1;
      cameraPosition.z += 1;
      break;
    case KeyEvent::KEY_d:
    case KeyEvent::KEY_RIGHT:
      cameraPosition.x += 1;
      cameraPosition.z -= 1;
      break;
    case KeyEvent::KEY_r:
      mWorld = InitialWorld();
      break;
    case KeyEvent::KEY_ESCAPE:
      quit();
      break;
    }
  }
private:
  gl::BatchRef mFloor, mRocket, mEndpoint;

  Rockets::SimulationOptions mOptions;
  Rockets::World mWorld;
  double elapseTime;
  bool follow = true;

  CameraPersp mCamera;
  CameraUi mCamUi;
  vec3 cameraPosition = vec3(30, 44, 30);
};

CINDER_APP(RocketsApp, RendererGl(RendererGl::Options().msaa(16)))
