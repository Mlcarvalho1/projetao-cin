# Simulação — Wokwi

Tudo que você precisa pra testar o firmware antes de ter o hardware na mão.

## Visão geral

| Cenário | Como rodar |
|---|---|
| TX sozinho (validar leitura ADC + envio UDP) | Wokwi simula TX; `udp-listener.mjs` na sua máquina recebe |
| RX sozinho (validar parser + uploader) | `udp-sender.mjs` na sua máquina envia frames válidos; Wokwi simula RX |
| TX + RX juntos | Dois Wokwi rodando lado a lado (ver "TX + RX") |

## Pré-requisitos

1. **PlatformIO** instalado (`pip install -U platformio` ou `brew install platformio`).
2. **Extensão Wokwi for VS Code** (Marketplace → "Wokwi Simulator"). Login gratuito necessário.
3. Buildar os UF2 antes de abrir o simulador:
   ```sh
   cd hardware/firmware/pico-tx && pio run -e pico-tx-wifi
   cd hardware/firmware/pico-rx && pio run -e pico-rx
   ```
   O Wokwi lê `.pio/build/<env>/firmware.uf2` (caminho já configurado no `wokwi.toml`).

## Rodando o TX no Wokwi

1. Abra `hardware/firmware/pico-tx/diagram.json` no VS Code.
2. **F1 → Wokwi: Start Simulator** (ou clique no ícone do play do Wokwi).
3. Você vê o Pico W com 3 potenciômetros — gira pra simular umidade variável.
4. O Serial monitor do Wokwi mostra os logs do firmware.

Pra ele enviar UDP pra algum lugar, configure no `pico-tx/include/secrets.h`:
```cpp
constexpr const char* RX_HOST = "<IP da sua máquina na LAN>";
```
E rode o listener no host:
```sh
PSK_HEX=$(cat psk.hex) node hardware/firmware/sim/udp-listener.mjs
```
> Wokwi conecta via gateway que NATea pra internet real, então pra alcançar sua máquina o IP precisa ser roteável (LAN funciona).

## Rodando o RX no Wokwi

1. Abra `hardware/firmware/pico-rx/diagram.json`.
2. **F1 → Wokwi: Start Simulator**.
3. O firmware vai conectar no WiFi (use `Wokwi-GUEST`/sem senha no `secrets.h` que o gateway aceita) e ouvir UDP.
4. Pra simular um TX enviando frames válidos:
   ```sh
   PSK_HEX=$(cat psk.hex) node hardware/firmware/sim/udp-sender.mjs \
       --host <IP_QUE_O_RX_RECEBEU_DO_WIFI> --port 4500 --interval 5
   ```
   O IP do RX aparece no Serial monitor quando ele conecta no WiFi.

## TX + RX juntos

Dois caminhos:

**(a) Duas instâncias do Wokwi VS Code lado a lado:** abra os dois `diagram.json` em duas janelas/abas. Cada Pico W ganha um IP diferente; o TX precisa do IP do RX no `secrets.h`. **Limitação:** os dois usam o mesmo gateway Wokwi mas a roteabilidade entre eles depende do plano (free pode bloquear). Confirmar no primeiro teste.

**(b) Wokwi (TX) + RX rodando no `udp-listener.mjs`:** mais confiável. O listener atua como o RX completo do ponto de vista do TX, e quando você for testar o RX de verdade, usa o `udp-sender.mjs`. Esse é o setup recomendado pra desenvolvimento iterativo.

## Gerar o PSK pra os scripts

```sh
openssl rand -hex 32 > psk.hex
# copie esses 64 chars hex pro PSK[32] dos secrets.h dos dois devices.
```

## Limites conhecidos do Wokwi pro nosso caso

- WiFi simulado tem latência maior que a real (~100ms+), e o gateway pode bloquear UDP outbound dependendo do plano.
- Deepsleep do RP2040 não é simulado fielmente — `delay()` em vez disso pra desenvolvimento. Validar consumo de bateria só na placa real.
- ADC: os potenciômetros do Wokwi mapeiam linearmente 0–65535. Suficiente pra testar normalização, não captura ruído real do ADC.
- VSYS / GP29 (battery sense do Pico W): **não simulado**. Use mock no firmware (`#ifdef WOKWI` retorna 3850 mV fixo) durante simulação.
