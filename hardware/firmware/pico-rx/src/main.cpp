// AquaSense — RX (receptor da fazenda). Esqueleto da etapa 1: imprime
// FRAME_SIZE e parâmetros de fila pra confirmar que o build + lib
// aquasense_proto estão linkando. WiFi + UDP listener + Uploader entram
// na etapa 3+.
#include <Arduino.h>

#include "aquasense/frame.h"
#include "config.h"

void setup() {
    Serial.begin(115200);
    const uint32_t t0 = millis();
    while (!Serial && millis() - t0 < 3000) {}

    Serial.println();
    Serial.println("aquasense pico-rx (MVP wifi/udp)");
    Serial.printf("FRAME_SIZE=%u  udp_port=%u\n",
                  unsigned(aquasense::FRAME_SIZE),
                  unsigned(aquasense::rx::config::UDP_PORT));
    Serial.printf("upload_queue=%u  dedup_window=%u\n",
                  unsigned(aquasense::rx::config::UPLOAD_QUEUE_CAPACITY),
                  unsigned(aquasense::rx::config::DEDUP_WINDOW));
#if AQUASENSE_TLS_VERIFY
    Serial.println("tls=verify");
#else
    Serial.println("tls=insecure (MVP only — flip AQUASENSE_TLS_VERIFY=1 before prod)");
#endif
}

void loop() {
    // TODO etapa 3: WiFi.connect, UDP listener, frame_unpack, dedupe.seen,
    //               Uploader.queue, Uploader.tick (POST /readings com retry/backoff).
    delay(1000);
}
