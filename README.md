# esphome-dmx512

This is a DMX512 custom component for ESPHome that allows Arduino-based devices to control DMX devices via UART (this requires an RS485 module, see below).

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

_Tip:_ Usage of `gamma_correct: 0` in the lights configuration is likely required for most fixtures, as the gamma compensation is usually already done in their hardware. This can be observed when, without this setting set to 0, dimming to around 10% will actually turn the lights off. ESPHome has `gamma_correct` set to `2.8` [by default](https://esphome.io/components/light/index.html).

## 16 bit outputs

Some fixtures support 16 bit outputs. Usually, two adjacent DMX channels are used for this. You can achieve such an effect by creating a `template`-output and using a lambda function for writing the state to your outputs. The example below uses channel 1 and 2 for a combined 16 bit output:

```
output:
- platform: dmx512
  channel: 1
  universe: dmx
  id: coarse_red
- platform: dmx512
  channel: 2
  universe: dmx
  id: fine_red
- platform: template
  id: red_output
  type: float
  min_power: 0.01
  write_action:
    - lambda: |-
        int lvl = static_cast<int>(state*65535.0);
        uint8_t low = lvl & 0xff;
        uint8_t high = (lvl >> 8);
        id(coarse_red).set_level(high/255.0);
        id(fine_red).set_level(low/255.0);
```

## Wiring

You can use an RS485-TTL adapter module to connect your ESP device with the DMX bus.

Don't forget about 120Ohm termination resistors. If your fixture has DMX IN and OUT ports, on the OUT port of the last fixture in the chain you should use a termination resistor between XLR pins 2 and 3. Similarly on your module, it has to be placed in parallel with A and B outputs, given that it's going to be placed at the start of the chain. Most of the modules already contain these resistors.

Using good quality 120Ohm impedance cables, DMX lines can be run a maximum distance of approximately 1000 meters. With CAT5 cable DMX lines are safe until approximately 300 meters.

### MAX3485

The MAX3485 is an 3.3V RS485-TTL adapter module.

```
MAX3485 VCC   ->   ESP +3.3V
MAX3485 TXD   ->   not connected
MAX3485 RXD   ->   ESP32 GPIO5 or ESP8266 GPIO2 (as per examples above)
MAX3485 GND   ->   ESP GND
MAX3485 D+/A  ->   XLR 3 (DMX +)
MAX3485 D-/B  ->   XLR 2 (DMX -)
MAX3485 GND   ->   XLR 1 (DMX GND)
```

### MAX485

The MAX485 is an 5V RS485-TTL adapter module.

***Attention: Sometimes the MAX485 module works on 3.3V, however there is no guarantee it works correctly. To be on the safe side, use the MAX3485 instead (which is the equivalent for 3.3V). NEVER power the module by 5V, the ESP is not designed for 5V logic!***

```
MAX485 VCC  ->   ESP +3.3V
MAX485 GND  ->   ESP GND
MAX485 DE   ->   ESP +3.3V
MAX485 RE   ->   not connected
MAX485 DI   ->   ESP32 GPIO5 or ESP8266 GPIO2 (as per examples above)
MAX485 A    ->   XLR 3 (DMX +)
MAX485 B    ->   XLR 2 (DMX -)
MAX485 GND  ->   XLR 1 (DMX GND)
```

The RE pin can be left unconnected, since we do not want to receive anything from the bus.

For this module, you could even leave DE unconnected since there is a pull-up resistor on the board. You can also tie the DE pin to a GPIO of the ESP. Usually, you would configure this GPIO as `enable_pin` in the DMX component to activate the module automatically.

If you want to have a "mute" switch instead, define it as a switch instead and do not configure `enable_pin` in the DMX component:

```
switch:
- platform: gpio
  name: 'DMX output MUTE'
  icon: mdi:lightbulb-off
  pin:
    number: GPIO13
    inverted: true
```

