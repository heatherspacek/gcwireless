#include "tx.h"

void setup_transmitter()
{
  pinMode(PIN_031, INPUT_PULLUP);

  pinMode(PIN_017, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // memory setup
  // AX, AY, CX, CY, LA, RA each 0..255 (1 byte each; 6b so far)
  // ABXYZSLR DDDD____
  tx_buffer[0] = 0x01;
  tx_buffer[1] = 0x04;
  tx_buffer[2] = 0xDD;
  tx_buffer[3] = 0xCC;
  tx_buffer[4] = 0xBB;
  tx_buffer[5] = 0xAA;
  tx_buffer[6] = 0x27;
  tx_buffer[7] = 0x27;
  tx_buffer[6] = 0x27;
  tx_buffer[7] = 0x27;
  tx_buffer[8] = 0x27;
  tx_buffer[9] = 0x27;
  nrf_radio_packetptr_set(NRF_RADIO, tx_buffer);

  // tx config
  nrf_radio_txpower_set(NRF_RADIO, NRF_RADIO_TXPOWER_POS8DBM);
  nrf_radio_frequency_set(NRF_RADIO, 2401);

  const nrf_radio_packet_conf_t packet_conf = {
    .lflen      = 8,                                    // 8-bit length field
    .s0len      = 1,                                    // 1 byte S0 field
    .s1len      = 0,                                    // No S1 field
    .s1incl     = false,                                // Don't include S1 in RAM
    .cilen      = 0,                                    // No code indicator
    .plen       = NRF_RADIO_PREAMBLE_LENGTH_8BIT,       // 8-bit preamble
    .crcinc     = false,                                // CRC not in length
    .termlen    = 0,                                    // No termination field
    .maxlen     = 8,                                    // Max payload 8 bytes
    .statlen    = 0,                                    // Use dynamic length
    .balen      = 4,                                    // 4-byte base address
    .big_endian = false,                                // Little endian
    .whiteen    = false                                 // No whitening
  };

  nrf_radio_packet_configure(NRF_RADIO, &packet_conf);
  nrf_radio_mode_set(NRF_RADIO, NRF_RADIO_MODE_NRF_2MBIT);

  // physical address setup:
  // addy0 uses base0 + prefix0.AP0 ()
  // addy1 uses base1 + 
  // see table 35 (601).
  nrf_radio_base0_set(NRF_RADIO, 0x12345678);
  nrf_radio_prefix0_set(NRF_RADIO, 0x12345678);

  // logical address select:
  uint8_t logical_addy = 0x00;
  nrf_radio_txaddress_set(NRF_RADIO, logical_addy);

  // SHORTCUTS!!
  // as soon as TX ramp-up is done, generate START.
//   nrf_radio_shorts_enable(NRF_RADIO, RADIO_SHORTS_READY_START_Msk);
  // as soon as packet is done, START again.
  // ** not sure if wanted?
//   nrf_radio_shorts_enable(NRF_RADIO, RADIO_SHORTS_END_START_Msk);

  for (int retry_count = 0; retry_count < 20 && !Serial; retry_count++)
  {
    delay(100);
  }
  nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_TXEN);
  while (!nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_TXREADY))
  {
    // spin.
    Serial.print("STUCK!!!");
  }
}

int64_t t_ctr = 0;

void loop_transmitter () {
  delay(1);
  t_ctr++;
  digitalWrite(LED_BUILTIN, ((t_ctr % 500) < 100));

  // check if we should power down
  if (!digitalRead(PIN_031)) {
    NRF_POWER->SYSTEMOFF = 1;
  }

}

void sense_light_and_send() 
{
  bool light = !digitalRead(PIN_017);
  digitalWrite(LED_BUILTIN, light);
  tx_buffer[0] = (uint8_t) light;  // promote and overwrite

  nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_START);
  while (!nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_END))
  {
    // spin.
  }
  nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_END);
}
