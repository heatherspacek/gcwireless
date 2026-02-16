#include "rx.h"

bool SERIAL_CONNECTED = false;

void setup_receiver()
{
  pinMode(PIN_017, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  nrf_radio_packetptr_set(NRF_RADIO, rx_buffer);
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

  // address setup
  nrf_radio_base0_set(NRF_RADIO, 0x12345678);
  nrf_radio_prefix0_set(NRF_RADIO, 0x12345678);

  // bit position sets listening/not-listening for each logical channel
  uint8_t rx_channel_enable = 0b00000001;
  nrf_radio_rxaddresses_set(NRF_RADIO, rx_channel_enable);

//   nrf_radio_shorts_enable(NRF_RADIO, NRF_RADIO_SHORT_RXREADY_START_MASK);
  nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_RXEN);  // start spinning up

  for (int retry_count = 0; retry_count < 20 && !Serial; retry_count++)
  {
    delay(100);
  }
  SERIAL_CONNECTED = (bool) Serial;
  Serial.printf("connected = %d", SERIAL_CONNECTED);

  while (!nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_RXREADY))
  {
    if (SERIAL_CONNECTED) {Serial.print("STUCK!!!");}
  }
  nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_RXREADY);
  nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_START);
  // Serial.print("done setup.");
}

int64_t ctr = 0;

void loop_receiver() {
  delay(1);
  // receive...  
  ctr++;
  if (SERIAL_CONNECTED && (ctr % 500 == 0)) {Serial.print("idling ... \n");}
  digitalWrite(LED_BUILTIN, (bool)((ctr % 500) < 250));

  // if (nrf_radio_event_check(NRF_RADIO, NRF_RADIO_EVENT_END))
  // {
  //   nrf_radio_event_clear(NRF_RADIO, NRF_RADIO_EVENT_END);
  //   uint8_t light = rx_buffer[0];
  //   digitalWrite(LED_BUILTIN, light);
  //   if (SERIAL_CONNECTED) {Serial.print("PKT RCV !!!");}
  //   nrf_radio_task_trigger(NRF_RADIO, NRF_RADIO_TASK_START);
  // }
//   Serial.print("no pkt. !!!");
}