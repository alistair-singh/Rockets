#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RocketsApp : public App {
public:
  void setup() override {
    // Set up the camera.
    mCamera.lookAt(vec3(30, 44, 30), vec3(0,20.0f,0));
    mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
    mCamUi = CameraUi(&mCamera, getWindow());

    // Create batches that render fast.x
    auto colorShader = gl::getStockShader(gl::ShaderDef().color().lambert());

    mFloor = gl::Batch::create(geom::Plane().size(vec2(30)).subdivisions(ivec2(30)), colorShader);
    mRef = gl::Batch::create(geom::Sphere(), colorShader);
    mCenter = gl::Batch::create(geom::Sphere(), colorShader);
  }

  void update() override {
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
    {
      gl::ScopedColor color(Color::hex(0x5492f7));
      gl::ScopedModelMatrix model;

      gl::multModelMatrix(glm::scale(vec3(0.5f)));
      mCenter->draw();
    }

    // Draw the end.
    {
      gl::ScopedColor color(Color::hex(0x5492f7));
      gl::ScopedModelMatrix model;

      gl::multModelMatrix(glm::translate(vec3(0, 35.0f, 0)) * glm::scale(vec3(0.5f)));
      mCenter->draw();
    }

    // Draw the ref.
    {
      gl::ScopedColor color(Color::hex(0xf44242));
      gl::ScopedModelMatrix model;

      gl::multModelMatrix(glm::translate(vec3(1,2,3)) * glm::scale(vec3(0.5f)));
      mRef->draw();
    }

    // Draw the floor.
    {
      gl::ScopedColor color(Color::hex(0x183818));
      mFloor->draw();
    }
  }

private:
  gl::BatchRef mFloor, mRef, mCenter;

  CameraPersp mCamera;
  CameraUi mCamUi;
};

CINDER_APP(RocketsApp, RendererGl(RendererGl::Options().msaa(16)))
