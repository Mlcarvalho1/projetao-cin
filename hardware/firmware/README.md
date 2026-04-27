# AquaSense — Firmware

Firmware C++ para os dois devices do projeto:

- **`pico-tx/`** — nó de campo (Pico W no MVP). Lê 3 sensores ADC, monta frame binário e envia.
- **`pico-rx/`** — receptor da fazenda (Pico W). Recebe, valida HMAC + CRC, dedupe, e POSTa em `/readings`.
- **`shared/`** — biblioteca PlatformIO (`aquasense_proto`) com o protocolo binário, HMAC-SHA256 e CRC16.
- **`sim/`** — Wokwi diagrams + scripts host (`udp-listener.mjs`, `udp-sender.mjs`) pra testar antes da PCB.

## MVP vs. produção

No MVP o link entre TX e RX é **WiFi/UDP**, não LoRa. Isso valida toda a topologia de 2 devices + servidor sem depender do driver LoRa nem da PCB. Quando migrarmos pra LoRa, mudam só dois arquivos por device (`wifi_tx.{h,cpp}` → `lora_tx.{h,cpp}` etc.) e o `board` no `platformio.ini` do TX volta a ser `pico` (sem WiFi).

TLS no RX está em modo `insecure` (sem validação de cert) controlado pelo flag `AQUASENSE_TLS_VERIFY` no `platformio.ini`. **Antes de produção: trocar para 1 e embutir o CA cert.**

## Pré-requisitos

```sh
# PlatformIO Core
pip install -U platformio
# ou via brew:
brew install platformio
```

## Build

```sh
# Lib compartilhada — testes nativos no host (sem hardware)
cd shared && pio test -e native

# TX
cd pico-tx && pio run -e pico-tx-wifi

# RX
cd pico-rx && pio run -e pico-rx
```

## Flash

Pressione **BOOTSEL** no Pico, conecte USB, e:

```sh
pio run -e pico-tx-wifi -t upload    # dentro de pico-tx/
pio run -e pico-rx      -t upload    # dentro de pico-rx/
```

Monitor serial:

```sh
pio device monitor -b 115200
```

## Secrets

Cada device tem `include/secrets.h.example` versionado. Antes de buildar:

```sh
cp pico-tx/include/secrets.h.example pico-tx/include/secrets.h
cp pico-rx/include/secrets.h.example pico-rx/include/secrets.h
# editar com WiFi creds, PSK (mesma nos dois), URL da API, etc.
```

A chave `PSK` deve ser **idêntica** entre TX e RX da mesma fazenda. Gerar:

```sh
openssl rand -hex 32
```

`secrets.h` está no `.gitignore`.

## Layout do frame (24 bytes, little-endian)

```
[device_id : 8B] [seq : 2B] [s1 : 2B] [s2 : 2B] [s3 : 2B] [bat_mv : 2B] [hmac : 4B] [crc16 : 2B]
```

- `device_id`: serial do RP2040 (`pico_get_unique_board_id`).
- `seq`: contador uint16 LE persistido em flash, sobrevive a reset/deepsleep.
- `s1/s2/s3`: ADC raw (12-bit padded a 16-bit). **Normalização é responsabilidade do servidor.**
- `bat_mv`: tensão da bateria em mV.
- `hmac`: HMAC-SHA256(payload, PSK), truncado aos primeiros 4 bytes.
- `crc16`: CRC-16/CCITT-FALSE sobre `[payload || hmac]`.

Ver `shared/include/aquasense/frame.h` para o contrato C++ e `shared/test/` para vetores de teste (FIPS 180-2, RFC 4231).

## Simulação (sem hardware)

Cada device tem `wokwi.toml` + `diagram.json` ao lado do `platformio.ini`. Com a extensão **Wokwi for VS Code**, abra o `diagram.json` e dê start no simulador — o TX vem com 3 potenciômetros nos pinos ADC simulando os sensores capacitivos.

Pra testar isolado sem precisar dos dois Pico Ws ao mesmo tempo:

```sh
# Fake RX: roda no host, valida HMAC e CRC dos frames vindos do TX no Wokwi.
PSK_HEX=$(openssl rand -hex 32) node sim/udp-listener.mjs

# Fake TX: roda no host, envia frames válidos pro RX simulado no Wokwi.
PSK_HEX=$SAME_PSK node sim/udp-sender.mjs --host <IP_DO_RX> --interval 5
```

Detalhes em `sim/README.md`.

## Status das etapas

- [x] **Etapa 1** — `shared/` (frame, CRC16, HMAC-SHA256) + testes nativos + Wokwi configs + scripts de teste.
- [ ] **Etapa 2** — TX: ADC + WiFi/UDP send.
- [ ] **Etapa 3** — RX: WiFi + UDP listener + Uploader (POST /readings com retry/backoff).
- [ ] **Etapa 4** — Persistência do `seq` em flash (TX).
- [ ] **Etapa 5** — Deepsleep no TX (`pico/sleep.h`).
- [ ] **Etapa 6** — Migração WiFi/UDP → LoRa (RadioLib + SX1276).
- [ ] **Etapa 7** — TLS com verificação + CA cert embutido (pré-produção).
