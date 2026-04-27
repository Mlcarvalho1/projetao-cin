#include <unity.h>

#include <cstdint>
#include <cstring>

#include "aquasense/crc16.h"
#include "aquasense/frame.h"
#include "aquasense/hmac.h"
#include "aquasense/sha256.h"

using namespace aquasense;

void setUp() {}
void tearDown() {}

// FIPS 180-2 vectors.
static void test_sha256_empty() {
    uint8_t out[32];
    Sha256::hash(nullptr, 0, out);
    const uint8_t expected[32] = {
        0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,
        0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,
        0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,
        0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55,
    };
    TEST_ASSERT_EQUAL_MEMORY(expected, out, 32);
}

static void test_sha256_abc() {
    uint8_t out[32];
    const uint8_t input[3] = {'a','b','c'};
    Sha256::hash(input, 3, out);
    const uint8_t expected[32] = {
        0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,
        0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
        0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,
        0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad,
    };
    TEST_ASSERT_EQUAL_MEMORY(expected, out, 32);
}

static void test_sha256_two_block() {
    // "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" (56 bytes)
    const uint8_t input[56] = {
        'a','b','c','d','b','c','d','e','c','d','e','f','d','e','f','g',
        'e','f','g','h','f','g','h','i','g','h','i','j','h','i','j','k',
        'i','j','k','l','j','k','l','m','k','l','m','n','l','m','n','o',
        'm','n','o','p','n','o','p','q',
    };
    uint8_t out[32];
    Sha256::hash(input, sizeof(input), out);
    const uint8_t expected[32] = {
        0x24,0x8d,0x6a,0x61,0xd2,0x06,0x38,0xb8,
        0xe5,0xc0,0x26,0x93,0x0c,0x3e,0x60,0x39,
        0xa3,0x3c,0xe4,0x59,0x64,0xff,0x21,0x67,
        0xf6,0xec,0xed,0xd4,0x19,0xdb,0x06,0xc1,
    };
    TEST_ASSERT_EQUAL_MEMORY(expected, out, 32);
}

// RFC 4231 test case 1.
static void test_hmac_rfc4231_case1() {
    const uint8_t key[20] = {
        0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
        0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
        0x0b,0x0b,0x0b,0x0b,
    };
    const uint8_t data[8] = {'H','i',' ','T','h','e','r','e'};
    uint8_t out[32];
    hmac_sha256(key, sizeof(key), data, sizeof(data), out);
    const uint8_t expected[32] = {
        0xb0,0x34,0x4c,0x61,0xd8,0xdb,0x38,0x53,
        0x5c,0xa8,0xaf,0xce,0xaf,0x0b,0xf1,0x2b,
        0x88,0x1d,0xc2,0x00,0xc9,0x83,0x3d,0xa7,
        0x26,0xe9,0x37,0x6c,0x2e,0x32,0xcf,0xf7,
    };
    TEST_ASSERT_EQUAL_MEMORY(expected, out, 32);
}

// RFC 4231 test case 4 (key shorter than block, longer payload).
static void test_hmac_rfc4231_case4() {
    uint8_t key[25];
    for (uint8_t i = 0; i < sizeof(key); ++i) key[i] = uint8_t(i + 1);
    uint8_t data[50];
    for (size_t i = 0; i < sizeof(data); ++i) data[i] = 0xcd;
    uint8_t out[32];
    hmac_sha256(key, sizeof(key), data, sizeof(data), out);
    const uint8_t expected[32] = {
        0x82,0x55,0x8a,0x38,0x9a,0x44,0x3c,0x0e,
        0xa4,0xcc,0x81,0x98,0x99,0xf2,0x08,0x3a,
        0x85,0xf0,0xfa,0xa3,0xe5,0x78,0xf8,0x07,
        0x7a,0x2e,0x3f,0xf4,0x67,0x29,0x66,0x5b,
    };
    TEST_ASSERT_EQUAL_MEMORY(expected, out, 32);
}

// CRC-16/CCITT-FALSE check value: "123456789" -> 0x29B1.
static void test_crc16_check_string() {
    const uint8_t input[9] = {'1','2','3','4','5','6','7','8','9'};
    TEST_ASSERT_EQUAL_HEX16(0x29B1, crc16(input, sizeof(input)));
}

static void test_crc16_empty() {
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, crc16(nullptr, 0));
}

// ct_equal must distinguish equal vs differing buffers.
static void test_ct_equal() {
    const uint8_t a[4] = {0x10, 0x20, 0x30, 0x40};
    const uint8_t b[4] = {0x10, 0x20, 0x30, 0x40};
    const uint8_t c[4] = {0x10, 0x20, 0x30, 0x41};
    TEST_ASSERT_TRUE(ct_equal(a, b, 4));
    TEST_ASSERT_FALSE(ct_equal(a, c, 4));
}

// Frame: pack -> unpack returns identical fields.
static void test_frame_roundtrip() {
    uint8_t psk[PSK_SIZE];
    for (size_t i = 0; i < PSK_SIZE; ++i) psk[i] = uint8_t(i);

    Frame f{};
    for (int i = 0; i < 8; ++i) f.device_id[i] = uint8_t(0xA0 + i);
    f.seq    = 1234;
    f.s1     = 50000;
    f.s2     = 1;
    f.s3     = 65535;
    f.bat_mv = 3850;

    uint8_t buf[FRAME_SIZE];
    frame_pack(f, psk, buf);

    Frame out{};
    TEST_ASSERT_EQUAL(int(FrameError::OK),
                      int(frame_unpack(buf, FRAME_SIZE, psk, out)));
    TEST_ASSERT_EQUAL_MEMORY(f.device_id, out.device_id, 8);
    TEST_ASSERT_EQUAL_UINT16(f.seq,    out.seq);
    TEST_ASSERT_EQUAL_UINT16(f.s1,     out.s1);
    TEST_ASSERT_EQUAL_UINT16(f.s2,     out.s2);
    TEST_ASSERT_EQUAL_UINT16(f.s3,     out.s3);
    TEST_ASSERT_EQUAL_UINT16(f.bat_mv, out.bat_mv);
}

static void test_frame_bad_length() {
    uint8_t psk[PSK_SIZE] = {};
    uint8_t buf[FRAME_SIZE - 1] = {};
    Frame out{};
    TEST_ASSERT_EQUAL(int(FrameError::BAD_LENGTH),
                      int(frame_unpack(buf, sizeof(buf), psk, out)));
}

// Tamper a payload byte AND recompute CRC: HMAC should be the only thing failing.
static void test_frame_tampered_payload_fails_hmac() {
    uint8_t psk[PSK_SIZE];
    for (size_t i = 0; i < PSK_SIZE; ++i) psk[i] = uint8_t(i);

    Frame f{};
    f.seq = 7; f.s1 = 100; f.s2 = 200; f.s3 = 300; f.bat_mv = 3700;

    uint8_t buf[FRAME_SIZE];
    frame_pack(f, psk, buf);

    buf[10] ^= 0x01; // flip a bit in s1
    const uint16_t new_crc = crc16(buf, FRAME_SIZE - CRC_SIZE);
    buf[FRAME_SIZE - 2] = uint8_t(new_crc);
    buf[FRAME_SIZE - 1] = uint8_t(new_crc >> 8);

    Frame out{};
    TEST_ASSERT_EQUAL(int(FrameError::BAD_HMAC),
                      int(frame_unpack(buf, FRAME_SIZE, psk, out)));
}

static void test_frame_tampered_crc() {
    uint8_t psk[PSK_SIZE];
    for (size_t i = 0; i < PSK_SIZE; ++i) psk[i] = uint8_t(i);

    Frame f{};
    f.seq = 7;
    uint8_t buf[FRAME_SIZE];
    frame_pack(f, psk, buf);
    buf[FRAME_SIZE - 2] ^= 0x01; // corrupt CRC

    Frame out{};
    TEST_ASSERT_EQUAL(int(FrameError::BAD_CRC),
                      int(frame_unpack(buf, FRAME_SIZE, psk, out)));
}

// Wrong PSK fails HMAC even if CRC checks out.
static void test_frame_wrong_psk_fails_hmac() {
    uint8_t psk_a[PSK_SIZE]; for (size_t i = 0; i < PSK_SIZE; ++i) psk_a[i] = uint8_t(i);
    uint8_t psk_b[PSK_SIZE]; for (size_t i = 0; i < PSK_SIZE; ++i) psk_b[i] = uint8_t(i + 1);

    Frame f{};
    f.seq = 9; f.bat_mv = 3500;
    uint8_t buf[FRAME_SIZE];
    frame_pack(f, psk_a, buf);

    Frame out{};
    TEST_ASSERT_EQUAL(int(FrameError::BAD_HMAC),
                      int(frame_unpack(buf, FRAME_SIZE, psk_b, out)));
}

int main(int /*argc*/, char** /*argv*/) {
    UNITY_BEGIN();
    RUN_TEST(test_sha256_empty);
    RUN_TEST(test_sha256_abc);
    RUN_TEST(test_sha256_two_block);
    RUN_TEST(test_hmac_rfc4231_case1);
    RUN_TEST(test_hmac_rfc4231_case4);
    RUN_TEST(test_crc16_check_string);
    RUN_TEST(test_crc16_empty);
    RUN_TEST(test_ct_equal);
    RUN_TEST(test_frame_roundtrip);
    RUN_TEST(test_frame_bad_length);
    RUN_TEST(test_frame_tampered_payload_fails_hmac);
    RUN_TEST(test_frame_tampered_crc);
    RUN_TEST(test_frame_wrong_psk_fails_hmac);
    return UNITY_END();
}
