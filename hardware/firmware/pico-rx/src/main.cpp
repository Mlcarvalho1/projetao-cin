// AquaSense — RX (Pico 2 W). Esqueleto da etapa 1: imprime FRAME_SIZE e
// parâmetros de config no USB-CDC pra confirmar que o build + lib
// aquasense_proto estão linkando. ADC + WiFi + Uploader entram nas próximas etapas.
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

    printf("\naquasense pico-rx (MVP: sensores direto via ADC + HTTPS POST)\n");
    printf("FRAME_SIZE=%u  PAYLOAD=%u  HMAC=%u  CRC=%u\n",
           unsigned(aquasense::FRAME_SIZE),
           unsigned(aquasense::PAYLOAD_SIZE),
           unsigned(aquasense::HMAC_TRUNC_SIZE),
           unsigned(aquasense::CRC_SIZE));
    printf("sample_interval=%us  upload_queue=%u\n",
           unsigned(aquasense::rx::config::SAMPLE_INTERVAL_SECONDS),
           unsigned(aquasense::rx::config::UPLOAD_QUEUE_CAPACITY));
#if AQUASENSE_TLS_VERIFY
    printf("tls=verify\n");
#else
    printf("tls=insecure (MVP only — flip AQUASENSE_TLS_VERIFY=1 antes de produção)\n");
#endif
    print_unique_id();

    while (true) {
        // TODO Etapa 2: ler ADC dos 3 sensores + bateria, montar Frame, frame_pack.
        // TODO Etapa 3: WiFi (cyw43_arch_init + connect), Uploader (POST /readings com retry/backoff).
        sleep_ms(1000);
    }
}
