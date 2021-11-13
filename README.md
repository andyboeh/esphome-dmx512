# esphome-dmx512

This is a DMX512 custom component for ESPHome that allows ESP32-Arduino-based
devices to control DMX devices via UART (this requires usually an RS485 module).

## Implementation

The implementation is based on other projects:

  * https://github.com/cupertinomiranda/esphome
  * https://github.com/jakosek/esphome/tree/dmx_no_uart

It differs in that it uses the internal UART component but generates the
break signal by detaching and re-attaching the GPIO pin. Since this requires
directly messing around with GPIO pins, it is limited to the Arduino ESP32 core.

## Configuration

See the provided example file. For the `dmx512` component, some options need
additonal explanation:
```
dmx512:
  id: dmx
  uart_id: uart_bus
  enable_pin: 33
  tx_pin: 5
  uart_num: 1
```

  * `id`: The ID of this DMX512 hub
  * `uart_id`: Set this to the ID of your UART component
  * `enable_pin`: Set this to the pin number the MAX585 enable pins are connected
  to. Optional
  * `tx_pin`: Set this to the same pin number as the UART component. This is required
  for the generation of the break signal. Defaults to 5.
  * `uart_num`: Set this to the internal ESP32 UART number. If only logging is
  configured, this should be set to 1 (default). 
