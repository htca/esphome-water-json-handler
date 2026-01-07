
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

# Namespace waarin de C++ class leeft:
water_json_ns = cg.esphome_ns.namespace("water_json_handler")
WaterJsonHandler = water_json_ns.class_("WaterJsonHandler", cg.Component)

# YAML keys voor je sensors
CONF_FLOW = "flow_sensor"
CONF_TOTAL = "total_sensor"

# Component-config: een ID (de C++ instantie) en twee sensor-IDs
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_ID): cv.declare_id(WaterJsonHandler),
    cv.Required(CONF_FLOW): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TOTAL): cv.use_id(sensor.Sensor),
})

# Codegen: instantie aanmaken, registreren en je sensors doorgeven aan set_sources()
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    flow = await cg.get_variable(config[CONF_FLOW])
    total = await cg.get_variable(config[CONF_TOTAL])
    cg.add(var.set_sources(flow, total))
