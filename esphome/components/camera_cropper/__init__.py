# __init__.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_HEIGHT,
    CONF_ID,
    CONF_WIDTH,
)

CODEOWNERS = ["@nliaudat"]

AUTO_LOAD = ["camera"]

CONF_CAMERA_ID = "camera_id"
CONF_CROP_X = "crop_x"
CONF_CROP_Y = "crop_y"
CONF_IMAGE_FORMAT = "image_format"
CONF_IMAGE_FORMAT_ID = "image_format_id"
CONF_IMAGE_ID = "image_id"

camera_ns = cg.esphome_ns.namespace("camera")
camera_cropper_ns = cg.esphome_ns.namespace("camera_cropper")

Processor = camera_ns.class_("Processor")
Camera = camera_ns.class_("CameraImpl")
CameraImageImpl = camera_ns.class_("CameraImageImpl")
CameraCropper = camera_cropper_ns.class_("CameraCropper", Processor)

CameraImageSpec = camera_ns.struct("CameraImageSpec")

ImageFormat = camera_ns.enum("ImageFormat")

CONF_IMAGE_FORMAT_SELECTS = {
    "GRAYSCALE": ImageFormat.IMAGE_FORMAT_GRAYSCALE,
    "RGB": ImageFormat.IMAGE_FORMAT_RGB,
    "RGB565": ImageFormat.IMAGE_FORMAT_RGB565,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_CAMERA_ID): cv.use_id(Camera),
        cv.Required(CONF_CROP_X): cv.int_range(min=0),
        cv.Required(CONF_CROP_Y): cv.int_range(min=0),
        cv.Required(CONF_WIDTH): cv.int_range(min=1),
        cv.Required(CONF_HEIGHT): cv.int_range(min=1),
        cv.Required(CONF_IMAGE_FORMAT): cv.enum(CONF_IMAGE_FORMAT_SELECTS, upper=True),
        cv.GenerateID(): cv.declare_id(CameraCropper),
        cv.GenerateID(CONF_IMAGE_ID): cv.declare_id(CameraImageImpl),
        cv.GenerateID(CONF_IMAGE_FORMAT_ID): cv.declare_id(CameraImageSpec),
    }
)

CONFIG_SCHEMA = cv.ensure_list(CONFIG_SCHEMA)


async def to_code(config):
    for conf in config:
        spec = cg.new_Pvariable(
            conf[CONF_IMAGE_FORMAT_ID],
            cg.StructInitializer(
                CameraImageSpec,
                ("width", conf[CONF_WIDTH]),
                ("height", conf[CONF_HEIGHT]),
                ("format", conf[CONF_IMAGE_FORMAT]),
            ),
        )
        image = cg.new_Pvariable(conf[CONF_IMAGE_ID])
        cg.add(image.set_data_length(spec.bytes_per_image()))
        
        cropper = cg.new_Pvariable(
            conf[CONF_ID],
            spec,
            image,
            conf[CONF_CROP_X],
            conf[CONF_CROP_Y],
            conf[CONF_WIDTH],
            conf[CONF_HEIGHT]
        )

        camera = await cg.get_variable(conf[CONF_CAMERA_ID])
        cg.add(camera.append_processor(cropper))
