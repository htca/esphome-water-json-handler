
# components/water_json_handler/__init__.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor as sensor_comp
from esphome.const import CONF_ID

water_json_ns = cg.esphome_ns.namespace("water_json_handler")
WaterJsonHandler = water_json_ns.class_("WaterJsonHandler", cg.Component)

CONF_FLOW = "flow_sensor"
CONF_TOTAL = "total_sensor"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(WaterJsonHandler),
        cv.Required(CONF_FLOW): cv.use_id(sensor_comp.Sensor),
        cv.Required(CONF_TOTAL): cv.use_id(sensor_comp.Sensor),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    flow = await cg.get_variable(config[CONF_FLOW])
    total = await cg.get_variable(config[CONF_TOTAL])
    cg.add(var.set_sources(flow, total))
``
