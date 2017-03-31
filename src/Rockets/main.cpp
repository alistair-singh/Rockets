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
    mCamera.lookAt(vec3(2.0f, 3.0f, 1.0f), vec3(0));
    mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 100.0f);
    mCamUi = CameraUi(&mCamera, getWindow());

    // Create batches that render fast.
    auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());
    auto colorShader = gl::getStockShader(gl::ShaderDef().color());

    mFloor = gl::Batch::create(geom::WirePlane().size(vec2(1000)).subdivisions(ivec2(1000)), colorShader);
  }

  void update() override {
  }

  void draw() override {
    // Gray background.
    gl::clear(Color::gray(0.5f));

    // Set up the camera.
    gl::ScopedMatrices push;
    gl::setMatrices(mCamera);

    // Enable depth buffer.
    gl::ScopedDepth depth(true);

    // Draw the grid on the floor.
    {
      gl::ScopedColor color(Color::gray(0.2f));
      mFloor->draw();
    }
  }

private:
  gl::BatchRef mFloor;

  CameraPersp mCamera;
  CameraUi mCamUi;
};

CINDER_APP(RocketsApp, RendererGl(RendererGl::Options().msaa(16)))
