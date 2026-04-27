#include "aquasense/frame.h"

#include <cstring>

#include "aquasense/crc16.h"
#include "aquasense/hmac.h"
#include "aquasense/sha256.h"

namespace aquasense {

namespace {

inline void put_u16_le(uint8_t* p, uint16_t v) {
    p[0] = uint8_t(v);
    p[1] = uint8_t(v >> 8);
}

inline uint16_t get_u16_le(const uint8_t* p) {
    return uint16_t(uint16_t(p[0]) | (uint16_t(p[1]) << 8));
}

constexpr size_t HMAC_OFFSET = PAYLOAD_SIZE;
constexpr size_t CRC_OFFSET  = PAYLOAD_SIZE + HMAC_TRUNC_SIZE;

} // namespace

void frame_pack(const Frame& f,
                const uint8_t psk[PSK_SIZE],
                uint8_t out[FRAME_SIZE]) {
    std::memcpy(out, f.device_id, 8);
    put_u16_le(out + 8,  f.seq);
    put_u16_le(out + 10, f.s1);
    put_u16_le(out + 12, f.s2);
    put_u16_le(out + 14, f.s3);
    put_u16_le(out + 16, f.bat_mv);

    uint8_t mac[Sha256::HASH_SIZE];
    hmac_sha256(psk, PSK_SIZE, out, PAYLOAD_SIZE, mac);
    std::memcpy(out + HMAC_OFFSET, mac, HMAC_TRUNC_SIZE);

    const uint16_t crc = crc16(out, CRC_OFFSET);
    put_u16_le(out + CRC_OFFSET, crc);
}

FrameError frame_unpack(const uint8_t* buf, size_t len,
                        const uint8_t psk[PSK_SIZE],
                        Frame& out) {
    if (len != FRAME_SIZE) return FrameError::BAD_LENGTH;

    const uint16_t expected_crc = crc16(buf, CRC_OFFSET);
    const uint16_t received_crc = get_u16_le(buf + CRC_OFFSET);
    if (expected_crc != received_crc) return FrameError::BAD_CRC;

    uint8_t mac[Sha256::HASH_SIZE];
    hmac_sha256(psk, PSK_SIZE, buf, PAYLOAD_SIZE, mac);
    if (!ct_equal(mac, buf + HMAC_OFFSET, HMAC_TRUNC_SIZE)) return FrameError::BAD_HMAC;

    std::memcpy(out.device_id, buf, 8);
    out.seq    = get_u16_le(buf + 8);
    out.s1     = get_u16_le(buf + 10);
    out.s2     = get_u16_le(buf + 12);
    out.s3     = get_u16_le(buf + 14);
    out.bat_mv = get_u16_le(buf + 16);
    return FrameError::OK;
}

} // namespace aquasense
