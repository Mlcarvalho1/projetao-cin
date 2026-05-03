# ADR 0003 — C++/PlatformIO em vez de MicroPython

- **Status:** aceito
- **Data:** 2026-05-03

## Contexto

O README original do projeto mencionava **MicroPython** como linguagem do firmware, que é a escolha mais comum para quem começa com Raspberry Pi Pico. MicroPython permite iteração rápida (REPL, sem compilação), tem boa documentação e possui bibliotecas para LoRa e sensores ADC. Porém, o projeto exige um frame binário compacto com HMAC-SHA256 e CRC-16, operações de baixo nível em ADC com oversampling, deep sleep real entre leituras, e watchdog timer — tudo isso é possível em MicroPython, mas com controle menor sobre memória e timing.

## Decisão

Usar **C++17 com PlatformIO** (toolchain `earlephilhower/arduino-pico` para o protótipo `pico-dht`; SDK nativo via `raspberrypi/pico` para `pico-rx` e `pico-tx`). As bibliotecas de criptografia (SHA-256, HMAC, CRC-16) são implementadas em C puro dentro da biblioteca compartilhada `aquasense_proto`, testadas nativamente via PlatformIO + Unity.

## Consequências

**Positivas**
- Controle total sobre o layout de memória e o timing do ADC.
- SHA-256 e HMAC-SHA256 implementados do zero em C — sem dependência de bibliotecas Python que podem mudar ou não estar disponíveis no firmware.
- PlatformIO tem suporte a testes nativos (`pio test -e native`), permitindo rodar a suíte de testes do protocolo sem hardware.
- `pico-dht` usa o framework Arduino (via `earlephilhower/arduino-pico`), o que mantém a DX familiar com `setup()`/`loop()`, WiFi, HTTPClient, etc.
- Compilação cruzada e flash via `pio run -t upload` sem precisar de IDE.

**Negativas**
- Ciclo de iteração mais lento: compile → flash → teste vs. REPL do MicroPython.
- Mais verboso: gerenciamento manual de buffers, ponteiros, etc.
- Curva de aprendizado maior para quem vem de Python.

## Alternativas consideradas

- **MicroPython** — descartado: implementar HMAC-SHA256 confiável em MicroPython puro é trabalhoso; o módulo `cryptolib` do MicroPython oficial não tem HMAC; a performance de deep sleep e ADC é menos previsível.
- **Arduino IDE (sem PlatformIO)** — descartado: sem suporte a monorepo, sem testes nativos, gerenciamento de bibliotecas manual.
- **Rust (embassy-rs)** — considerado mas fora do escopo do MVP pela complexidade do ecossistema e tempo de onboarding.
