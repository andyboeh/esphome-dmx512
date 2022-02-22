#pragma once

#ifdef USE_ESP_IDF

#include "dmx512.h"

namespace esphome {
namespace dmx512 {

class DMX512ESP32IDF : public DMX512 {
 public:
  DMX512ESP32IDF() = default;

  void send_break() override;

  void set_uart_num(int num) override {
    this->uart_idx_ = num;
  }
};

}  // namespace dmx512
}  // namespace esphome

#endif  // USE_ESP_IDF
