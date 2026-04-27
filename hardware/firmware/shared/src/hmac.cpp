#include "aquasense/hmac.h"

#include <cstring>

#include "aquasense/sha256.h"

namespace aquasense {

void hmac_sha256(const uint8_t* key, size_t key_len,
                 const uint8_t* data, size_t data_len,
                 uint8_t out[32]) {
    constexpr size_t B = Sha256::BLOCK_SIZE;
    uint8_t k[B] = {};
    if (key_len > B) {
        Sha256::hash(key, key_len, k);
    } else {
        std::memcpy(k, key, key_len);
    }

    uint8_t ipad[B], opad[B];
    for (size_t i = 0; i < B; ++i) {
        ipad[i] = uint8_t(k[i] ^ 0x36);
        opad[i] = uint8_t(k[i] ^ 0x5c);
    }

    uint8_t inner[Sha256::HASH_SIZE];
    {
        Sha256 h;
        h.update(ipad, B);
        h.update(data, data_len);
        h.finalize(inner);
    }

    Sha256 h;
    h.update(opad, B);
    h.update(inner, sizeof(inner));
    h.finalize(out);
}

bool ct_equal(const uint8_t* a, const uint8_t* b, size_t len) {
    uint8_t diff = 0;
    for (size_t i = 0; i < len; ++i) diff |= uint8_t(a[i] ^ b[i]);
    return diff == 0;
}

} // namespace aquasense
