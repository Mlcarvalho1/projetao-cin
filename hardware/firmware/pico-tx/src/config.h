#pragma once

#include <cstddef>
#include <cstdint>

// Placeholder pra Etapa 6: este TX só entra em jogo quando migrarmos pro link LoRa.
// MVP roda 100% no RX (com sensores conectados direto).
namespace aquasense::tx::config {

// Intervalo entre leituras. Pico dorme entre amostras (Etapa 5+6).
constexpr uint32_t SLEEP_SECONDS = 30;

// Pinos ADC do RP2040: GP26 (ADC0), GP27 (ADC1), GP28 (ADC2). GP29/ADC3 reservado pra bateria.
constexpr uint8_t SENSOR_PIN_S1 = 26;
constexpr uint8_t SENSOR_PIN_S2 = 27;
constexpr uint8_t SENSOR_PIN_S3 = 28;
constexpr uint8_t BATTERY_PIN   = 29;

// Oversampling pra reduzir ruído do ADC.
constexpr uint8_t ADC_OVERSAMPLE = 8;

// Tentativas de transmissão antes de desistir e voltar a dormir.
constexpr uint8_t TX_MAX_RETRIES = 3;

// LoRa params — preencher quando o módulo (SX1276 ou similar) estiver definido.
// constexpr uint32_t LORA_FREQ_HZ = 915000000;
// constexpr uint8_t  LORA_SF      = 7;
// constexpr uint8_t  LORA_BW_KHZ  = 125;

} // namespace aquasense::tx::config
