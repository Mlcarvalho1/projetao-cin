#pragma once

#include <cstddef>
#include <cstdint>

namespace aquasense {

class Sha256 {
public:
    static constexpr size_t HASH_SIZE  = 32;
    static constexpr size_t BLOCK_SIZE = 64;

    Sha256();
    void update(const uint8_t* data, size_t len);
    void finalize(uint8_t out[HASH_SIZE]);

    static void hash(const uint8_t* data, size_t len, uint8_t out[HASH_SIZE]);

private:
    uint32_t state_[8];
    uint64_t bit_count_;
    uint8_t  buffer_[BLOCK_SIZE];
    size_t   buffer_len_;

    void compress(const uint8_t block[BLOCK_SIZE]);
};

} // namespace aquasense
