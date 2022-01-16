# esphome-dmx512

This is a DMX512 custom component for ESPHome that allows Arduino-based devices to control DMX devices via UART (this requires an RS485 module, see below). ESP32 with the IDF-framework is currently not supported.

## Implementation

The implementation is based on other projects:

  * https://github.com/cupertinomiranda/esphome
  * https://github.com/jakosek/esphome/tree/dmx_no_uart

It differs in that it uses the internal UART component but generates the break signal by detaching and re-attaching the GPIO pin (on ESP32).

## Important notes

DMX only works on hardware UARTs, therefore the number of the UART in use needs to be set. This defaults to 1, as the first UART is usually used for logging purposes.

On the ESP8266, the only pin you can use is GPIO2. It's TX-only and can easily be used for DMX.

## Configuration

See the provided [example file](example_dmx.yaml) for a complete example, including `output` and `light` components. For the `dmx512` component, some options need additional explanation:
```
dmx512:
  id: dmx
  uart_id: uart_bus
  enable_pin: GPIO33
  tx_pin: GPIO5
  uart_num: 1
  periodic_update: true
  force_full_frames: false
  custom_break_len: 92
  custom_mab_len: 12
  update_interval: 500
```

  * `id`: The ID of this DMX512 hub
  * `uart_id`: Set this to the ID of your UART component
  * `enable_pin`: Set this to the pin number the MAX585 enable pins are connected to. Optional
  * `tx_pin`: Set this to the same pin number as the UART component. This is required for the generation of the break signal. Defaults to GPIO5.
  * `uart_num`: Set this to the internal ESP32 UART number. If only logging is configured, this should be set to 1 (default). 
  * `periodic_update`: If set to false, only state changes are transmitted and the bus is silent in between - violates the specification and may cause some dimmers to turn off
  * `force_full_frames`: If set to true, the full 513-byte frame is always sent. Otherwise, only the configured channels are transmitted.
  * `custom_mab_len`: Set a custom mark-after-break length (in uS, default 12)
  * `custom_break_len`: Set a custom break length (in uS, default 92)
  * `update_interval`: Specify a custom update interval, i.e. the minimum time between resending the current values (in ms, default 500)

Outputs point to channels in the DMX universe, from 1 to 512. 

Some DMX fixtures can be daisy chained with XLR cables, and each one can be set to operate at a different address. When you specify the output channel in the configuration of this component, take into account that _channel (in esphome component) = address + channel (on the fixture)_. 

When you choose addresses for your DMX fixtures keep in mind that the next free address number on the bus that can be assigned is the _address of the previous light + the last channel number of that light_. 

For easier understanding, eg. the [ADJ VBar Pak](https://d295jznhem2tn9.cloudfront.net/ItemRelatedFiles/8659/vbar_pak.pdf) can be used in various modes. When you have 4 of these daisy chained, if you set them up to 8-channel mode (to access all the effects) you can set the address of the first one to 1, the next one to 9, the third one to 17, fourth one to 25, using the buttons on each fixture. 

Then in the config you'd have channels for the first one from 1 to 8, the second one from 9 to 16, third one from 17 to 24, forth one from 25 to 32. 

See [another example](example_4x_adj_vbar_pak.yaml) for this kind of configuration.

The `output` implements float values between 0 and 100. You can use it not only for `light`, but for any component which can send data to it, moreover, it can be set from lambdas. 

The above example shows use cases for this, where certain outputs are used for hardware effects selection on the DMX fixture, using a `select` component, and parameters can be adjusted from `number` entities.

## Wiring

You can use an RS485-TTL adapter module to connect your ESP device with the DMX bus.

![MAX485-M](https://user-images.githubusercontent.com/1550668/149642143-7e13fb00-29fd-4e9d-8f11-6b4a2a2bd0ba.png)

For this adapter, use the wiring below:

```
MAX485-M VCC     -> ESP +3.3V
MAX485-M GND     -> ESP GND
MAX485-M RE & DE -> ESP +3.3V
MAX485-M DI      -> ESP32 GPIO5 or ESP8266 GPIO2 (as per examples above)
MAX485-M A       -> XLR 3 (DMX +)
MAX485-M B       -> XLR 2 (DMX -)
MAX485-M GND     -> XLR 1 (DMX GND)
```

Don't forget about 120Ohm termination resistors. If your fixture has DMX IN and OUT ports, on the OUT port of the last fixture in the chain you should use a termination resistor between XLR pins 2 and 3. Similarly on MAX485-M, it has to be placed in parallel with A and B outputs, given that it's going to be placed at the start of the chain.

Using good quality 120Ohm impedance cables, DMX lines can be run a maximum distance of approximately 1000 meters. With CAT5 cable DMX lines are safe until approximately 300 meters.