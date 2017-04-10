#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Rockets.h"

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
    cell.boosters = vec4(random() + 9.81 + 0.35,random() + 9.81 + 0.35,random() + 9.81 + 0.35, random() + 9.81 + 0.35);
    //cell.booster1 = vec3(0, 19.81 + 0.50, 0);
    //cell.booster1 = vec3(.15, 9.81 + 0.50, .15);
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
  world.rocket.bounds = vec3(1, 4, 1);
  world.rocket.position = vec3(0, 1.5, 0);
  world.rocket.mass = 1.0f;
  world.rocket.inertiaTensorBody = world.rocket.calculateInertiaTensorBody();
  world.rocket.invertedInertiaTensorBody = glm::inverse(world.rocket.inertiaTensorBody);
  world.rocket.feul = makeFeulCells(10000);
  return world;
}

class RocketsApp : public App {
public:
  void setup() override {
    // Set up the camera.
    mCamera.lookAt(vec3(30, 44, 30), vec3(0, 20.0f, 0));
    mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 100000);
    mCamUi = CameraUi(&mCamera, getWindow());

    // Create batches that render fast.
    auto colorShader = gl::getStockShader(gl::ShaderDef().color());
    auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());

    mFloor = gl::Batch::create(geom::Plane().size(vec2(30)).subdivisions(ivec2(30)), colorShader);
    mRocket = gl::Batch::create(geom::Cube(), lambertShader);
    mEndpoint = gl::Batch::create(geom::Sphere(), lambertShader);

    auto textureShader = gl::getStockShader(gl::ShaderDef().texture());
    mText = gl::Batch::create(geom::Rect().rect(messageBox), textureShader);

    mWorld = InitialWorld();
  }

  void update() override {
    if (!pause) {
      auto currentTime = getElapsedSeconds();
      while ((currentTime - elapseTime) > mOptions.timeStep) {
        for(int i = 0; i < speed; i++) Rockets::StepInPlace(mOptions, mWorld);
        elapseTime += mOptions.timeStep;
      }
    }

    if (follow) {
      mCamera.lookAt(mWorld.rocket.position);
    }

    std::ostringstream str;
    auto feulLeft = mWorld.rocket.feul.size() - mWorld.rocket.feulUsed;
    auto rotationMatrix = mWorld.rocket.rotationMatrix();

    auto boosters = vec4(0);
    auto boosterTorque = vec3(0);
    auto boosterForce = vec3(0);
    if (feulLeft > 0)
    {
      boosters = mWorld.rocket.feul[mWorld.rocket.feulUsed].boosters;
      boosterTorque = mWorld.rocket.feul[mWorld.rocket.feulUsed].torque(rotationMatrix);
      boosterForce = mWorld.rocket.feul[mWorld.rocket.feulUsed].force(rotationMatrix);
    }

    str
      << "Simulation: " << (pause ? "Paused" : "Running" ) << '\n'
      << "World Size (B): " << sizeof(mWorld) << "\n"
      << "Speed: " << speed << "\n"
      << "Camera Position: " << mCamera.getEyePoint() << '\n'
      << "Camera Direction: " << mCamera.getViewDirection() << '\n'
      << "Time Step: " << mOptions.timeStep << " s\n"
      << "Friction Coefficient: " << mOptions.frictionCoefficient << '\n'
      << "Gravity: " << mOptions.gravity << '\n'
      << "Time: " << mWorld.worldTime << '\n'
      << "Steps: " << mWorld.steps << '\n'
      << "Position: " << mWorld.rocket.position << '\n'
      << "Rotation: " << mWorld.rocket.rotation << '\n'
      << "Rotation Matrix: " << rotationMatrix << '\n'
      << "Momentum: " << mWorld.rocket.momentum << '\n'
      << "Velocity: " << mWorld.rocket.velocity() << '\n'
      << "Inertia Body Tensor: " << mWorld.rocket.inertiaTensorBody << '\n'
      << "Angular Momentum: " << mWorld.rocket.angularMomentum << '\n'
      << "Angular Velocity: " << mWorld.rocket.angularVelocity() << '\n'
      << "Boosters: " << boosters << '\n'
      << "Torque: " << boosterTorque << '\n'
      << "Force: " << boosterForce << '\n'
      << "Feul: " << feulLeft << '\n'
      << "Mass: " << mWorld.rocket.mass << '\n';
    TextBox tbox = TextBox().alignment(TextBox::LEFT).font(Font("Consolas", 14)).size(messageBox.getSize()).text(str.str());
    auto sz = tbox.render();
    mTexture = gl::Texture::create(sz);
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

      auto m1 = glm::mat4_cast(mWorld.rocket.rotation);
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

    gl::ScopedMatrices p1;
    gl::setMatricesWindow(getWindowSize());
    {
      gl::ScopedTextureBind text(mTexture);
      mText->draw();
    }
  }

  void keyDown(KeyEvent ev) override {
    switch (ev.getCode())
    {
    case KeyEvent::KEY_SPACE:
      follow = false;
      mCamera.lookAt(vec3(30, 44, 30), vec3(0, 20.0f, 0));
      break;
    case KeyEvent::KEY_f:
      follow = !follow;
      break;
    case KeyEvent::KEY_EQUALS:
      if (++speed > 0) { if (pause) { pause = false; elapseTime = getElapsedSeconds(); } };
      break;
    case KeyEvent::KEY_MINUS:
      if (--speed < 0) { speed = 0; pause = true; };
      break;
    case KeyEvent::KEY_s:
      pause = true;
      speed = 1;
      Rockets::StepInPlace(mOptions, mWorld);
      break;
    case KeyEvent::KEY_r:
      mWorld = InitialWorld();
      break;
    case KeyEvent::KEY_p:
      if (!(pause = !pause)) { elapseTime = getElapsedSeconds(); speed = 1; }
      break;
    case KeyEvent::KEY_ESCAPE:
    case KeyEvent::KEY_q:
      quit();
      break;
    }
  }
private:
  gl::BatchRef mFloor, mRocket, mEndpoint, mText;
  gl::Texture2dRef mTexture;
  Rectf messageBox = Rectf(5, 5, 155*3, 155 * 3);

  Rockets::SimulationOptions mOptions;
  Rockets::World mWorld;
  double elapseTime;
  bool follow = true;
  bool pause = false;
  int speed = 1;

  CameraPersp mCamera;
  CameraUi mCamUi;
};

CINDER_APP(RocketsApp, RendererGl(RendererGl::Options().msaa(16)))
