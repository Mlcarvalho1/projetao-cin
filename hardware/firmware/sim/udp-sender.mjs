#!/usr/bin/env node
// Fake TX para testar o RX em isolamento.
// Gera frames válidos (HMAC + CRC corretos) e envia via UDP num intervalo.
// PSK deve ser idêntica à do RX (`pico-rx/include/secrets.h`).
//
// Uso:
//   PSK_HEX=$(cat psk.hex) node udp-sender.mjs --host 192.168.1.50
//   PSK_HEX=... node udp-sender.mjs --host 127.0.0.1 --port 4500 --interval 30

import dgram from 'node:dgram';
import crypto from 'node:crypto';

const args = Object.fromEntries(
    process.argv.slice(2).reduce((acc, a, i, arr) => {
        if (a.startsWith('--')) acc.push([a.slice(2), arr[i + 1]]);
        return acc;
    }, [])
);

const HOST     = args.host ?? '127.0.0.1';
const PORT     = Number.parseInt(args.port ?? '4500', 10);
const INTERVAL = Number.parseInt(args.interval ?? '30', 10);
const COUNT    = args.count ? Number.parseInt(args.count, 10) : Infinity;

const PSK_HEX = process.env.PSK_HEX;
if (!PSK_HEX) { console.error('Defina PSK_HEX (32 bytes hex).'); process.exit(1); }
const psk = Buffer.from(PSK_HEX, 'hex');
if (psk.length !== 32) { console.error(`PSK_HEX deve ter 64 hex chars, recebeu ${psk.length * 2}.`); process.exit(1); }

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

const deviceId = crypto.randomBytes(8);
let seq = 0;

function buildFrame() {
    const buf = Buffer.alloc(FRAME_SIZE);
    deviceId.copy(buf, 0);
    buf.writeUInt16LE(seq & 0xffff, 8);
    // Simula umidade variando senoidalmente em torno de 32768.
    const t = Date.now() / 60000;
    buf.writeUInt16LE(Math.floor(32768 + 20000 * Math.sin(t)),       10);
    buf.writeUInt16LE(Math.floor(32768 + 18000 * Math.sin(t + 1.0)), 12);
    buf.writeUInt16LE(Math.floor(32768 + 22000 * Math.sin(t + 2.0)), 14);
    buf.writeUInt16LE(3850, 16); // bat_mv

    const mac = crypto.createHmac('sha256', psk).update(buf.subarray(0, PAYLOAD_SIZE)).digest();
    mac.copy(buf, PAYLOAD_SIZE, 0, HMAC_SIZE);

    const crc = crc16ccitt(buf.subarray(0, PAYLOAD_SIZE + HMAC_SIZE));
    buf.writeUInt16LE(crc, PAYLOAD_SIZE + HMAC_SIZE);
    return buf;
}

const sock = dgram.createSocket('udp4');
console.log(`udp-sender: enviando para ${HOST}:${PORT} a cada ${INTERVAL}s ` +
            `(device_id=${deviceId.toString('hex')})`);

let sent = 0;
function tick() {
    const frame = buildFrame();
    sock.send(frame, PORT, HOST, (err) => {
        const ts = new Date().toISOString();
        if (err) console.error(`[${ts}] erro:`, err.message);
        else console.log(`[${ts}] seq=${seq} → ${HOST}:${PORT} (${frame.length}B)`);
        seq = (seq + 1) & 0xffff;
        sent++;
        if (sent >= COUNT) { sock.close(); process.exit(0); }
    });
}

tick();
const timer = setInterval(tick, INTERVAL * 1000);
process.on('SIGINT', () => { clearInterval(timer); sock.close(); process.exit(0); });
