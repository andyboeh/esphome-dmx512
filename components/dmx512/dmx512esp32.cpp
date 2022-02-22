#ifdef USE_ESP32_FRAMEWORK_ARDUINO

#include "dmx512esp32.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dmx512 {

static const char *TAG = "dmx512";

void DMX512ESP32::send_break() {
  uint8_t num = this->tx_pin_->get_pin();
  pinMatrixOutDetach(num, false, false);
  pinMode(num, OUTPUT); // NOLINT
  digitalWrite(num, LOW); // NOLINT
  delayMicroseconds(this->break_len_);
  digitalWrite(num, HIGH); // NOLINT
  delayMicroseconds(this->mab_len_);
  pinMatrixOutAttach(num, this->uart_idx_, false, false);
}

}  // namespace dmx512
}  // namespace esphome
#endif  // USE_ESP32_FRAMEWORK_ARDUINO
