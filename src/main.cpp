#include <Arduino.h>
#include "nrf_radio.h"
// https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/group_nrf_radio_hal.html

#ifdef TX_MODE
#include "tx.h"
void setup() {setup_transmitter();}; void loop() {loop_transmitter();};
#else
#include "rx.h"
void setup() {setup_receiver();}; void loop() {loop_receiver();};
#endif
