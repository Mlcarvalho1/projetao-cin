#pragma once

#include <cstddef>
#include <cstdint>

namespace aquasense::rx::config {

// MVP: sensores capacitivos conectados direto no RX via ADC.
// Etapa 6 (LoRa): pinos de ADC migram pro pico-tx; aqui ficam só fila/uploader.

// Pinos ADC do RP2350: GP26 (ADC0), GP27 (ADC1), GP28 (ADC2). GP29/ADC3 reservado pra bateria.
constexpr uint8_t SENSOR_PIN_S1 = 26;
constexpr uint8_t SENSOR_PIN_S2 = 27;
constexpr uint8_t SENSOR_PIN_S3 = 28;
constexpr uint8_t BATTERY_PIN   = 29;

// Quantas amostras tomar e mediar por leitura, pra reduzir ruído do ADC.
constexpr uint8_t  ADC_OVERSAMPLE = 8;

// Intervalo entre ciclos (sensores → frame_pack → POST).
constexpr uint32_t SAMPLE_INTERVAL_SECONDS = 30;

// Fila do Uploader (FIFO drop quando enche). 128 frames * 24B = 3 KB de RAM,
// cobre ~64 min de outage com leituras a 30 s.
constexpr size_t UPLOAD_QUEUE_CAPACITY = 128;

// Backoff do Uploader em caso de falha de POST.
constexpr uint32_t UPLOAD_RETRY_INITIAL_MS = 1000;
constexpr uint32_t UPLOAD_RETRY_MAX_MS     = 30000;

// Watchdog: se o loop principal travar mais que isso, reset.
constexpr uint32_t WATCHDOG_TIMEOUT_MS = 8000;

// Reservado pra Etapa 6: dedup por device quando tiver vários TX por LoRa.
constexpr size_t DEDUP_WINDOW = 64;

} // namespace aquasense::rx::config
