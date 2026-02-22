#include "Relay.h"

typedef struct {
  gpio_num_t gpio;
  bool active_high;
  bool state;
  bool initialized;
} relay_t;

static relay_t s_relays[MAX_RELAYS] = {0};

void relay_init(uint8_t relay_idx, gpio_num_t relay_gpio, bool active_high) {
  if (relay_idx >= MAX_RELAYS) {
    return; // Index out of range
  }

  s_relays[relay_idx].gpio = relay_gpio;
  s_relays[relay_idx].active_high = active_high;
  s_relays[relay_idx].state = false;
  s_relays[relay_idx].initialized = true;

  gpio_config_t io_conf = {.pin_bit_mask = 1ULL << relay_gpio,
                           .mode = GPIO_MODE_OUTPUT,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .pull_up_en = GPIO_PULLUP_DISABLE,
                           .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&io_conf);

  relay_off(relay_idx); // Mặc định tắt
}

void relay_on(uint8_t relay_idx) {
  if (relay_idx >= MAX_RELAYS || !s_relays[relay_idx].initialized) {
    return;
  }

  gpio_set_level(s_relays[relay_idx].gpio,
                 s_relays[relay_idx].active_high ? 1 : 0);
  s_relays[relay_idx].state = true;
}

void relay_off(uint8_t relay_idx) {
  if (relay_idx >= MAX_RELAYS || !s_relays[relay_idx].initialized) {
    return;
  }

  gpio_set_level(s_relays[relay_idx].gpio,
                 s_relays[relay_idx].active_high ? 0 : 1);
  s_relays[relay_idx].state = false;
}

void relay_toggle(uint8_t relay_idx) {
  if (relay_idx >= MAX_RELAYS || !s_relays[relay_idx].initialized) {
    return;
  }

  if (s_relays[relay_idx].state) {
    relay_off(relay_idx);
  } else {
    relay_on(relay_idx);
  }
}

bool relay_get_state(uint8_t relay_idx) {
  if (relay_idx >= MAX_RELAYS || !s_relays[relay_idx].initialized) {
    return false;
  }

  return s_relays[relay_idx].state;
}
