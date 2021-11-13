#include "dmx512.h"
#include "esphome/core/log.h"
#include "esp32-hal-matrix.h"

namespace esphome {
namespace dmx512 {

static const char *TAG = "dmx512";

void DMX512::loop() {
    this->uart_->flush();
    this->sendBreak();
	device_values[0] = 0;
	this->uart_->write_array(device_values, 513);
}

void DMX512::sendBreak() {
    pinMatrixOutDetach(this->tx_pin_, false, false);
    pinMode(this->tx_pin_, OUTPUT);
    digitalWrite(this->tx_pin_, LOW);
    delayMicroseconds(88);
    digitalWrite(this->tx_pin_, HIGH);
    delayMicroseconds(1);
    pinMatrixOutAttach(this->tx_pin_, this->uart_idx_, false, false);
}

void DMX512::dump_config() {
    ESP_LOGCONFIG(TAG, "Setting up DMX512...");
}

void DMX512::setup() {
  for(int i = 0; i < 513; i++)
    this->device_values[i] = 0;
  if(this->pin_enable_) {
    ESP_LOGD(TAG, "Enabling RS485 module");
    this->pin_enable_->setup();
    this->pin_enable_->digital_write(true);
  }
}

void DMX512::write_channel(uint16_t channel, uint8_t value) {
  ESP_LOGD(TAG, "write_channel %d: %d", channel, value);
  this->device_values[channel] = value;
}

void DMX512Output::write_state(float state) {
  uint16_t value = state * 0xffff;
  this->universe_->write_channel(this->channel_, (value >> 8));
}

}  // namespace dmx512
}  // namespace esphome
