#ifdef USE_ESP_IDF

#include "dmx512esp32idf.h"
#include "esphome/core/log.h"
#include <driver/uart.h>

namespace esphome {
namespace dmx512 {

static const char *TAG = "dmx512";

void DMX512ESP32IDF::send_break() {
  uart_set_line_inverse(this->uart_idx_, UART_SIGNAL_TXD_INV);
  delayMicroseconds(this->break_len_);
  uart_set_line_inverse(this->uart_idx_, UART_SIGNAL_INV_DISABLE);
  delayMicroseconds(this->mab_len_);
}

}  // namespace dmx512
}  // namespace esphome
#endif  // USE_ESP_IDF
