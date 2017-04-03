#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Rockets.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Rockets::World InitialWorld() {
  Rockets::World world;
  Rockets::FeulCell cell;
  cell.booster1 = vec3(0, 10.5, 0);
  world.rocket.feul.push_back(cell);
  cell.booster1 = vec3(0, 10, 1);
  world.rocket.feul.push_back(cell);
  cell.booster1 = vec3(1, 8, -1);
  world.rocket.feul.push_back(cell);
  cell.booster1 = vec3(-1, 11, 0);
  world.rocket.feul.push_back(cell);
  cell.booster1 = vec3(0, 20, 0);
  world.rocket.feul.push_back(cell);
  return world;
}

class RocketsApp : public App {
public:
  void setup() override {
    // Set up the camera.
    mCamera.lookAt(vec3(30, 44, 30), vec3(0,20.0f,0));
    mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 100000);
    mCamUi = CameraUi(&mCamera, getWindow());

    // Create batches that render fast.
    auto colorShader = gl::getStockShader(gl::ShaderDef().color());
    auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());

    mFloor = gl::Batch::create(geom::Plane().size(vec2(30)).subdivisions(ivec2(30)), colorShader);
    mRocket = gl::Batch::create(geom::Sphere().radius(0.3f), lambertShader);
    mEndpoint = gl::Batch::create(geom::Sphere(), lambertShader);

    mWorld = InitialWorld();
  }

  void update() override {

    auto currentTime = getElapsedSeconds();
    while ((currentTime - elapseTime) > mOptions.timeStep) {
      mWorld = Rockets::Step(mOptions, mWorld);
      elapseTime += mOptions.timeStep;
    }

    if (follow) {
      mCamera.lookAt(vec3(30, 44, 30), mWorld.rocket.object.object.position);
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

      gl::multModelMatrix(glm::translate(mWorld.rocket.object.object.position));
      mRocket->draw();
    }

    // Draw the floor.
    {
      gl::ScopedColor color(Color::hex(0x183818));
      mFloor->draw();
    }
  }

  void keyUp(KeyEvent ev) override {
    switch (ev.getCode())
    {
    case KeyEvent::KEY_SPACE:
      follow = false;
      mCamera.lookAt(vec3(30, 44, 30), vec3(0, 20.0f, 0));
      break;
    case KeyEvent::KEY_f:
      follow = !follow;
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
  bool follow = false;

  CameraPersp mCamera;
  CameraUi mCamUi;
};

CINDER_APP(RocketsApp, RendererGl(RendererGl::Options().msaa(16)))
