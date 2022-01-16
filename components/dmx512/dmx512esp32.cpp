#ifdef USE_ESP32_FRAMEWORK_ARDUINO

#include "dmx512esp32.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dmx512 {

static const char *TAG = "dmx512";

void DMX512ESP32::sendBreak() {
  pinMatrixOutDetach(this->tx_pin_, false, false);
  pinMode(this->tx_pin_, OUTPUT);
  digitalWrite(this->tx_pin_, LOW);
  delayMicroseconds(this->break_len_);
  digitalWrite(this->tx_pin_, HIGH);
  delayMicroseconds(this->mab_len_);
  pinMatrixOutAttach(this->tx_pin_, this->uart_idx_, false, false);
}

}  // namespace dmx512
}  // namespace esphome
#endif  // USE_ESP32_FRAMEWORK_ARDUINO
