#!/usr/bin/env node
// Fake RX para testar o TX em isolamento.
// Bind UDP em 0.0.0.0:<port> e dumpa cada frame recebido. Se PSK_HEX estiver
// definido no env, valida HMAC truncado (4B) e CRC-16/CCITT-FALSE (2B).
//
// Uso:
//   node udp-listener.mjs                       # porta 4500, sem validação
//   PORT=4500 node udp-listener.mjs
//   PSK_HEX=$(cat psk.hex) node udp-listener.mjs # valida HMAC + CRC
//
// No simulador Wokwi, configure no secrets.h do TX:
//   RX_HOST = "<IP da sua máquina na LAN>"
//   UDP_PORT = 4500

import dgram from 'node:dgram';
import crypto from 'node:crypto';

const PORT = Number.parseInt(process.env.PORT ?? '4500', 10);
const PSK_HEX = process.env.PSK_HEX;
const psk = PSK_HEX ? Buffer.from(PSK_HEX, 'hex') : null;
if (psk && psk.length !== 32) {
    console.error(`PSK_HEX deve ter 32 bytes (64 hex chars), recebeu ${psk.length}.`);
    process.exit(1);
}

const FRAME_SIZE = 24;
const PAYLOAD_SIZE = 18;
const HMAC_SIZE = 4;

function crc16ccitt(buf) {
    let crc = 0xffff;
    for (const b of buf) {
        crc ^= b << 8;
        for (let i = 0; i < 8; i++) {
            crc = (crc & 0x8000) ? ((crc << 1) ^ 0x1021) & 0xffff : (crc << 1) & 0xffff;
        }
    }
    return crc;
}

function parseFrame(buf) {
    if (buf.length !== FRAME_SIZE) {
        return { ok: false, reason: `bad length: got ${buf.length}, expected ${FRAME_SIZE}` };
    }
    const expectedCrc = crc16ccitt(buf.subarray(0, PAYLOAD_SIZE + HMAC_SIZE));
    const receivedCrc = buf.readUInt16LE(PAYLOAD_SIZE + HMAC_SIZE);
    if (expectedCrc !== receivedCrc) {
        return { ok: false, reason: `bad CRC: got 0x${receivedCrc.toString(16)}, expected 0x${expectedCrc.toString(16)}` };
    }
    if (psk) {
        const mac = crypto.createHmac('sha256', psk).update(buf.subarray(0, PAYLOAD_SIZE)).digest();
        if (!crypto.timingSafeEqual(mac.subarray(0, HMAC_SIZE), buf.subarray(PAYLOAD_SIZE, PAYLOAD_SIZE + HMAC_SIZE))) {
            return { ok: false, reason: 'bad HMAC' };
        }
    }
    return {
        ok: true,
        device_id: buf.subarray(0, 8).toString('hex'),
        seq:    buf.readUInt16LE(8),
        s1:     buf.readUInt16LE(10),
        s2:     buf.readUInt16LE(12),
        s3:     buf.readUInt16LE(14),
        bat_mv: buf.readUInt16LE(16),
        hmac_check: psk ? 'verified' : 'skipped (no PSK_HEX)',
    };
}

const sock = dgram.createSocket('udp4');

sock.on('message', (msg, rinfo) => {
    const ts = new Date().toISOString();
    const parsed = parseFrame(msg);
    console.log(`\n[${ts}] ${rinfo.address}:${rinfo.port}  ${msg.length}B  ${msg.toString('hex')}`);
    if (parsed.ok) {
        console.log(`  device_id=${parsed.device_id}  seq=${parsed.seq}  ` +
                    `s1=${parsed.s1} s2=${parsed.s2} s3=${parsed.s3}  ` +
                    `bat=${parsed.bat_mv}mV  hmac=${parsed.hmac_check}`);
    } else {
        console.log(`  REJECTED: ${parsed.reason}`);
    }
});

sock.on('listening', () => {
    const a = sock.address();
    console.log(`udp-listener: ouvindo em ${a.address}:${a.port}` +
                (psk ? ' (validando HMAC + CRC)' : ' (apenas dump, sem PSK)'));
});

sock.bind(PORT);

process.on('SIGINT', () => { sock.close(); process.exit(0); });
