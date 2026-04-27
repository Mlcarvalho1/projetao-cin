#pragma once

#include <cstddef>
#include <cstdint>

namespace aquasense {

// CRC-16/CCITT-FALSE: poly 0x1021, init 0xFFFF, no reflection, xorout 0x0000.
uint16_t crc16(const uint8_t* data, size_t len);

} // namespace aquasense
