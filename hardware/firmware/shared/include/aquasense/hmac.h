#pragma once

#include <cstddef>
#include <cstdint>

namespace aquasense {

void hmac_sha256(const uint8_t* key, size_t key_len,
                 const uint8_t* data, size_t data_len,
                 uint8_t out[32]);

bool ct_equal(const uint8_t* a, const uint8_t* b, size_t len);

} // namespace aquasense
