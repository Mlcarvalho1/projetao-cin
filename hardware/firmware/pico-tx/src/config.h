#pragma once

#include <cstddef>
#include <cstdint>

namespace aquasense::tx::config {

// Intervalo entre leituras. RP2040 dorme entre amostras (etapa 5).
constexpr uint32_t SLEEP_SECONDS = 30;

// Pinos ADC do RP2040: GP26 (ADC0), GP27 (ADC1), GP28 (ADC2). GP29/ADC3 reservado pra bateria.
constexpr uint8_t SENSOR_PIN_S1 = 26;
constexpr uint8_t SENSOR_PIN_S2 = 27;
constexpr uint8_t SENSOR_PIN_S3 = 28;
constexpr uint8_t BATTERY_PIN   = 29;

// MVP WiFi/UDP (Leitura A). RX recebe frame binário cru no UDP.
constexpr uint16_t UDP_PORT = 4500;

// Quantas amostras tomar e mediar por leitura, pra reduzir ruído do ADC.
constexpr uint8_t  ADC_OVERSAMPLE = 8;

// Tentativas de transmissão antes de desistir e voltar a dormir.
constexpr uint8_t  TX_MAX_RETRIES = 3;

} // namespace aquasense::tx::config
