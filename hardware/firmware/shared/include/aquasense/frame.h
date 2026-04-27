#pragma once

#include <cstddef>
#include <cstdint>

namespace aquasense {

// Wire layout (little-endian):
//   [device_id:8] [seq:2] [s1:2] [s2:2] [s3:2] [bat_mv:2] [hmac:4] [crc16:2]
// Total: 24 bytes. Sensor values are raw uint16 (RP2040 ADC 12-bit padded to 16).
constexpr size_t PSK_SIZE        = 32;
constexpr size_t HMAC_TRUNC_SIZE = 4;
constexpr size_t CRC_SIZE        = 2;
constexpr size_t PAYLOAD_SIZE    = 18;
constexpr size_t FRAME_SIZE      = PAYLOAD_SIZE + HMAC_TRUNC_SIZE + CRC_SIZE;

struct Frame {
    uint8_t  device_id[8];
    uint16_t seq;
    uint16_t s1;
    uint16_t s2;
    uint16_t s3;
    uint16_t bat_mv;
};

enum class FrameError {
    OK,
    BAD_LENGTH,
    BAD_CRC,
    BAD_HMAC,
};

void frame_pack(const Frame& f,
                const uint8_t psk[PSK_SIZE],
                uint8_t out[FRAME_SIZE]);

FrameError frame_unpack(const uint8_t* buf, size_t len,
                        const uint8_t psk[PSK_SIZE],
                        Frame& out);

} // namespace aquasense
