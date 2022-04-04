#include "dmx512.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dmx512 {

static const char *TAG = "dmx512";

void DMX512::loop() {
  bool update = false;
  // this->update_ is true if something has changed
  // Otherwise, an update needs to be triggered periodically, if the
  // periodic update option is set
  if(this->update_ || (((millis() - this->last_update_) > this->update_interval_) && this->periodic_update_)) {
    // Force the refresh rate to be within the spec
    // Flushing should not be needed (we do it anyway)
    if((millis() - this->last_update_) > DMX_MIN_INTERVAL_MS) {
      update = true;
    }
  }
  if(update) {
    this->uart_->flush();
    this->send_break();
    this->device_values_[0] = 0;
    this->uart_->write_array(this->device_values_, this->max_chan_ + 1);
    this->update_ = false;
    this->last_update_ = millis();
  }
}

void DMX512::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up DMX512...");
}

void DMX512::setup() {
  for(int i = 0; i < DMX_MSG_SIZE; i++)
    this->device_values_[i] = 0;
  if(this->pin_enable_) {
    ESP_LOGD(TAG, "Enabling RS485 module");
    this->pin_enable_->setup();
    this->pin_enable_->digital_write(true);
  }
}

void DMX512::set_channel_used(uint16_t channel) {
  if(channel > this->max_chan_)
    this->max_chan_ = channel;
  if(this->force_full_frames_)
    this->max_chan_ = DMX_MAX_CHANNEL;
}

void DMX512::write_channel(uint16_t channel, uint8_t value) {
  ESP_LOGD(TAG, "write_channel %d: %d", channel, value);
  this->device_values_[channel] = value;
  this->update_ = true;
}

void DMX512Output::set_channel(uint16_t channel) {
  this->channel_ = channel;
  if(this->universe_) {
    this->universe_->set_channel_used(channel);
  }
}

void DMX512Output::write_state(float state) {
  this->state = state;
  uint16_t value = state * 0xffff;
  if(this->universe_)
    this->universe_->write_channel(this->channel_, (value >> 8));
}

}  // namespace dmx512
}  // namespace esphome
