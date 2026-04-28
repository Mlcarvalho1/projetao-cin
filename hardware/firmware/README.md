# AquaSense — Firmware

Firmware C/C++ (Pico SDK) para os dois devices do projeto:

- **`pico-rx/`** — receptor da fazenda (Pico 2 W, RP2350). **Device principal do MVP**: lê os 3 sensores via ADC, monta frame binário, valida HMAC + CRC localmente e POSTa em `/readings`.
- **`pico-tx/`** — nó de campo (Pico, RP2040 sem WiFi). Placeholder até a Etapa 6: vai ler sensores e mandar frames via LoRa pro RX.
- **`shared/`** — biblioteca PlatformIO (`aquasense_proto`) com o protocolo binário, HMAC-SHA256 e CRC16.
- **`sim/`** — Wokwi diagrams + scripts host (`udp-listener.mjs`, `udp-sender.mjs`) pra testar antes da PCB.

## MVP vs. produção

No MVP só o RX está em jogo: sensores conectados direto via ADC, sem rádio entre devices. Isso valida a topologia "device → API" sem depender do driver LoRa nem da PCB final. Quando o link LoRa entrar (Etapa 6), o RX vira apenas receptor e os pinos de ADC migram pro `pico-tx`.

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

# RX (device principal)
cd pico-rx && pio run -e pico-rx

# TX (placeholder até a Etapa 6)
cd pico-tx && pio run -e pico-tx
```

## Flash

Pressione **BOOTSEL** no Pico, conecte USB, e:

```sh
pio run -e pico-rx -t upload    # dentro de pico-rx/
pio run -e pico-tx -t upload    # dentro de pico-tx/
```

Monitor serial (USB-CDC, configurado via `PICO_STDIO_USB=1` no platformio.ini):

```sh
pio device monitor -b 115200
```

## Secrets

Cada device tem `include/secrets.h.example` versionado. Antes de buildar:

```sh
cp pico-rx/include/secrets.h.example pico-rx/include/secrets.h
cp pico-tx/include/secrets.h.example pico-tx/include/secrets.h
# editar com WiFi creds (RX), URL da API, PSK (mesma nos dois — só vira ativa na Etapa 6).
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

- `device_id`: serial do RP2040/RP2350 (`pico_get_unique_board_id`).
- `seq`: contador uint16 LE persistido em flash, sobrevive a reset/deepsleep.
- `s1/s2/s3`: ADC raw (12-bit padded a 16-bit). **Normalização é responsabilidade do servidor.**
- `bat_mv`: tensão da bateria em mV.
- `hmac`: HMAC-SHA256(payload, PSK), truncado aos primeiros 4 bytes.
- `crc16`: CRC-16/CCITT-FALSE sobre `[payload || hmac]`.

Ver `shared/include/aquasense/frame.h` para o contrato C++ e `shared/test/` para vetores de teste (FIPS 180-2, RFC 4231).

## Simulação (sem hardware)

Cada device tem `wokwi.toml` + `diagram.json` ao lado do `platformio.ini`. Com a extensão **Wokwi for VS Code**, abra o `diagram.json` e dê start no simulador — o RX vem com 3 potenciômetros nos pinos ADC simulando os sensores capacitivos.

> Wokwi ainda não tem parte para `rpipico2w`; o `diagram.json` do RX usa `wokwi-pi-pico-w` (RP2040+W) como aproximação. Os pinos ADC e WiFi são compatíveis pro propósito da simulação.

Pra testar isolado (útil quando o link LoRa entrar):

```sh
# Fake RX: roda no host, valida HMAC e CRC dos frames vindos do TX.
PSK_HEX=$(openssl rand -hex 32) node sim/udp-listener.mjs

# Fake TX: roda no host, envia frames válidos pro RX simulado.
PSK_HEX=$SAME_PSK node sim/udp-sender.mjs --host <IP_DO_RX> --interval 5
```

Detalhes em `sim/README.md`.

## Status das etapas

- [x] **Etapa 1** — `shared/` (frame, CRC16, HMAC-SHA256) + testes nativos + esqueletos `pico-rx`/`pico-tx` (picosdk).
- [ ] **Etapa 2** — RX: ADC + oversampling + frame_pack.
- [ ] **Etapa 3** — RX: WiFi (cyw43_arch) + Uploader (POST /readings com retry/backoff).
- [ ] **Etapa 4** — Persistência do `seq` em flash (RX no MVP, TX depois).
- [ ] **Etapa 5** — Deepsleep entre amostras.
- [ ] **Etapa 6** — TX entra em jogo: link LoRa (RadioLib + SX1276) entre `pico-tx` e `pico-rx`.
- [ ] **Etapa 7** — TLS com verificação + CA cert embutido (pré-produção).
