
# components/water_json_page/__init__.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID
from esphome.components import sensor

CONF_FLOW_SENSOR = "flow_sensor"
CONF_TOTAL_SENSOR = "total_sensor"

ns = cg.esphome_ns.namespace("water_json_page")
WaterJsonPage = ns.class_("WaterJsonPage", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WaterJsonPage),
    cv.Required(CONF_FLOW_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_TOTAL_SENSOR): cv.use_id(sensor.Sensor),
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    flow = await cg.get_variable(config[CONF_FLOW_SENSOR])
    total = await cg.get_variable(config[CONF_TOTAL_SENSOR])
    cg.add(var.set_flow_sensor(flow))
    cg.add(var.set_total_sensor(total))
