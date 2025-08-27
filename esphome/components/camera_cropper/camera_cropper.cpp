// camera_cropper.cpp
#include "camera_cropper.h"
#include "esphome/core/log.h"
#include "esphome/components/camera/camera.h"

namespace esphome {
namespace camera_cropper {

static const char *const TAG = "camera.cropper";

void CameraCropper::apply(std::shared_ptr<camera::CameraImage> source_image) {
  if (source_image == nullptr) {
    ESP_LOGW(TAG, "Source image is null");
    return;
  }

  // Validate crop region
  if (crop_x_ + crop_width_ > source_image->get_width() || 
      crop_y_ + crop_height_ > source_image->get_height()) {
    ESP_LOGE(TAG, "Crop region exceeds source image dimensions!");
    return;
  }

  // Prepare destination image
  image_->set_width(crop_width_);
  image_->set_height(crop_height_);
  image_->set_data_type(spec_->format);

  // Get source image data
  const uint8_t *source_data = source_image->get_data_buffer();
  size_t source_width = source_image->get_width();
  size_t source_height = source_image->get_height();

  // Calculate bytes per pixel based on image type
  size_t bytes_per_pixel = 1; // Default for grayscale
  if (spec_->format == camera::IMAGE_FORMAT_RGB) {
    bytes_per_pixel = 3;
  } else if (spec_->format == camera::IMAGE_FORMAT_RGB565) {
    bytes_per_pixel = 2;
  }

  // Get destination buffer
  uint8_t *dest_data = image_->get_data_buffer();
  size_t dest_size = image_->get_data_length();

  // Perform cropping
  for (size_t y = 0; y < crop_height_; y++) {
    size_t source_y = crop_y_ + y;
    if (source_y >= source_height) continue;
    
    for (size_t x = 0; x < crop_width_; x++) {
      size_t source_x = crop_x_ + x;
      if (source_x >= source_width) continue;
      
      size_t source_idx = (source_y * source_width + source_x) * bytes_per_pixel;
      size_t dest_idx = (y * crop_width_ + x) * bytes_per_pixel;
      
      // Check bounds
      if (source_idx + bytes_per_pixel <= source_image->get_data_length() && 
          dest_idx + bytes_per_pixel <= dest_size) {
        memcpy(dest_data + dest_idx, source_data + source_idx, bytes_per_pixel);
      }
    }
  }

  // Set the processed image
  this->set_image(image_);
}

}  // namespace camera_cropper
}  // namespace esphome