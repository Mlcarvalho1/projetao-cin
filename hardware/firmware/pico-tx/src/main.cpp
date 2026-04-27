// AquaSense — TX (nó de campo). Esqueleto da etapa 1: imprime FRAME_SIZE
// e device_id no Serial pra confirmar que o build + lib aquasense_proto
// estão linkando. Lógica de medição/transmissão entra na etapa 2+.
#include <Arduino.h>

#include "aquasense/frame.h"
#include "config.h"

namespace {

void print_unique_id() {
    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);
    Serial.print("device_id=");
    for (size_t i = 0; i < sizeof(id.id); ++i) {
        if (id.id[i] < 0x10) Serial.print('0');
        Serial.print(id.id[i], HEX);
    }
    Serial.println();
}

} // namespace

void setup() {
    Serial.begin(115200);
    const uint32_t t0 = millis();
    while (!Serial && millis() - t0 < 3000) {}

    Serial.println();
    Serial.println("aquasense pico-tx (MVP wifi/udp)");
    Serial.printf("FRAME_SIZE=%u  PAYLOAD=%u  HMAC=%u  CRC=%u\n",
                  unsigned(aquasense::FRAME_SIZE),
                  unsigned(aquasense::PAYLOAD_SIZE),
                  unsigned(aquasense::HMAC_TRUNC_SIZE),
                  unsigned(aquasense::CRC_SIZE));
    Serial.printf("sleep_seconds=%u\n",
                  unsigned(aquasense::tx::config::SLEEP_SECONDS));
    print_unique_id();
}

void loop() {
    // TODO etapa 2: ler ADC dos 3 sensores + bateria, montar Frame, frame_pack,
    //               WiFiUDP enviar para RX_HOST:UDP_PORT, retry, deepsleep.
    delay(1000);
}
