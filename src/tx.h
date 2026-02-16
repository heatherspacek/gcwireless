#include <Arduino.h>
#include "nrf_radio.h"
// https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/group_nrf_radio_hal.html

static uint8_t tx_buffer[10] __attribute__((aligned(4)));

void setup_transmitter();
void loop_transmitter();
