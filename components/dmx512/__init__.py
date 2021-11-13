import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins, automation
from esphome.components import uart
from esphome.const import CONF_ID, CONF_TX_PIN
from esphome.core import CORE, coroutine

DEPENDENCIES = ['uart']
MULTI_CONF = True

dmx512_ns = cg.esphome_ns.namespace('dmx512')
DMX512 = dmx512_ns.class_('DMX512', cg.Component)

CONF_DMX512_ID = 'dmx512_id'
CONF_ENABLE_PIN = 'enable_pin'
CONF_UART_NUM = 'uart_num'

def _declare_type(value):
    if CORE.is_esp32:
        if CORE.using_arduino:
            return cv.declare_id(DMX512)(value)
    raise NotImplementedError

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): _declare_type,
    cv.Optional(CONF_ENABLE_PIN): pins.gpio_output_pin_schema,
    cv.Optional(CONF_TX_PIN, default=5): cv.int_range(min=0,max=39),
    cv.Optional(CONF_UART_NUM, default=1): cv.int_range(min=0, max=2),
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    cg.add_global(dmx512_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    if CONF_ENABLE_PIN in config:
        enable = await cg.gpio_pin_expression(config[CONF_ENABLE_PIN])
        cg.add(var.set_enable_pin(enable))

    cg.add(var.set_uart_tx_pin(config[CONF_TX_PIN]))
    cg.add(var.set_uart_num(config[CONF_UART_NUM]))
