#pragma once

#include <cstddef>
#include <cstdint>

namespace aquasense::rx::config {

// MVP WiFi/UDP — TX envia frame binário cru pra esta porta.
constexpr uint16_t UDP_PORT = 4500;

// Janela de dedup por device: número de seqs recentes mantidos em RAM.
constexpr size_t DEDUP_WINDOW = 64;

// Fila do Uploader (FIFO drop quando enche). 128 frames * 24B = 3 KB de RAM,
// cobre ~64 min de outage com 1 TX a 30 s.
constexpr size_t UPLOAD_QUEUE_CAPACITY = 128;

// Backoff do Uploader em caso de falha de POST.
constexpr uint32_t UPLOAD_RETRY_INITIAL_MS = 1000;
constexpr uint32_t UPLOAD_RETRY_MAX_MS     = 30000;

// Watchdog: se o loop principal travar mais que isso, reset.
constexpr uint32_t WATCHDOG_TIMEOUT_MS = 8000;

} // namespace aquasense::rx::config
