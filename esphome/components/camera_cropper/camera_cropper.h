// camera_cropper.h
#pragma once

#include "esphome/components/camera/camera.h"
#include "esphome/core/component.h"

namespace esphome {
namespace camera_cropper {

using namespace camera;

class CameraCropper : public Processor {
 public:
  CameraCropper(CameraImageSpec *spec, CameraImageImpl *image, int crop_x, int crop_y, int crop_width, int crop_height)
      : spec_(spec), image_(image), crop_x_(crop_x), crop_y_(crop_y), crop_width_(crop_width), crop_height_(crop_height) {}

  void set_image(CameraImageImpl *image) { image_ = image; }
  void set_spec(CameraImageSpec *spec) { spec_ = spec; }

  void apply(std::shared_ptr<CameraImage> image) override;

 protected:
  CameraImageSpec *spec_;
  CameraImageImpl *image_;
  int crop_x_;
  int crop_y_;
  int crop_width_;
  int crop_height_;
};

}  // namespace camera_cropper
}  // namespace esphome