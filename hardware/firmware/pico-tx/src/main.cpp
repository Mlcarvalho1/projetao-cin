// AquaSense — TX (Pico, sem WiFi). Placeholder pra Etapa 6 (LoRa).
// Por enquanto só imprime FRAME_SIZE + device_id pra confirmar que o build
// e a lib aquasense_proto continuam linkando no RP2040 sem WiFi.
#include <cstdio>

#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "aquasense/frame.h"
#include "config.h"

namespace {

void print_unique_id() {
    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);
    printf("device_id=");
    for (size_t i = 0; i < sizeof(id.id); ++i) {
        printf("%02x", id.id[i]);
    }
    printf("\n");
}

} // namespace

int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("\naquasense pico-tx (placeholder — Etapa 6: LoRa)\n");
    printf("FRAME_SIZE=%u  PAYLOAD=%u\n",
           unsigned(aquasense::FRAME_SIZE),
           unsigned(aquasense::PAYLOAD_SIZE));
    printf("sleep_seconds=%u\n",
           unsigned(aquasense::tx::config::SLEEP_SECONDS));
    print_unique_id();

    while (true) {
        // TODO Etapa 6: ler ADC, frame_pack, transmitir via LoRa pro RX.
        sleep_ms(1000);
    }
}
