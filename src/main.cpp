#include <Arduino.h>

#include "nrf_radio.h"
// https://docs.nordicsemi.com/bundle/sdk_nrf5_v17.1.0/page/group_nrf_radio_hal.html


auto radio_reg = NRF_RADIO;

void setup()
{
  pinMode(PIN_017, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // easydma setup
  // AX, AY, CX, CY, LA, RA each 0..255 (1 byte each; 6b so far)
  // ABXYZSLR DDDD____

  static uint8_t tx_buffer[64] __attribute__((aligned(4)));
  // NRF_RADIO->PACKETPTR = NULL;
  //or...
  nrf_radio_packetptr_set(radio_reg, tx_buffer);

  // tx config
  nrf_radio_txpower_set(radio_reg, NRF_RADIO_TXPOWER_POS8DBM);
  nrf_radio_frequency_set(radio_reg, 2401);

  const nrf_radio_packet_conf_t packet_conf = {
    .lflen      = 8,                                    // 8-bit length field
    .s0len      = 1,                                    // 1 byte S0 field
    .s1len      = 0,                                    // No S1 field
    .s1incl     = false,                                // Don't include S1 in RAM
    .cilen      = 0,                                    // No code indicator
    .plen       = NRF_RADIO_PREAMBLE_LENGTH_8BIT,       // 8-bit preamble
    .crcinc     = false,                                // CRC not in length
    .termlen    = 0,                                    // No termination field
    .maxlen     = 255,                                  // Max payload 255 bytes
    .statlen    = 0,                                    // Use dynamic length
    .balen      = 3,                                    // 3-byte base address
    .big_endian = false,                                // Little endian
    .whiteen    = false                                 // No whitening
  };

  nrf_radio_packet_configure(radio_reg, &packet_conf);

  nrf_radio_mode_set(radio_reg, NRF_RADIO_MODE_NRF_2MBIT);


  // SHORTCUTS!!
  nrf_radio_shorts_enable(radio_reg, RADIO_SHORTS_END_START_Msk);

}

void loop()
{
  delay(20);
  digitalWrite(LED_BUILTIN, !digitalRead(PIN_017));

  nrf_radio_task_trigger(radio_reg, NRF_RADIO_TASK_TXEN);
  nrf_radio_task_trigger(radio_reg, NRF_RADIO_TASK_START);

}