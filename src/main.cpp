#include <Arduino.h>

#include "nrf_radio.h"
// https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/group_nrf_radio_hal.html

void setup()
{
  pinMode(PIN_017, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  auto radio_reg = NRF_RADIO;
  nrf_radio_frequency_set(radio_reg, 2401);
}

void loop()
{
  delay(20);
  digitalWrite(LED_BUILTIN, !digitalRead(PIN_017));
  
}