# ADR 0007 — Protótipo pico-dht com WiFi direto (Etapa 0)

- **Status:** aceito
- **Data:** 2026-05-03

## Contexto

A arquitetura alvo do AquaSense envolve dois dispositivos (Pico TX no campo + Pico RX com WiFi na casa) comunicando via LoRa, o que requer hardware adicional (módulo SX1276/RFM95), configuração de rádio e implementação das Etapas 2–6 do roadmap. Antes de investir nisso, havia duas incertezas importantes a validar:

1. **Contrato da API:** o `POST /readings` funciona corretamente com hardware real?
2. **Leitura dos sensores:** o sensor de umidade HW-080 e o DHT11 produzem dados confiáveis no contexto real?

Implementar LoRa antes de validar esses pontos atrasaria o feedback e arriscava descobrir problemas de API ou sensor depois de ter mais código para alterar.

## Decisão

Construir o **protótipo `pico-dht`** (Etapa 0): um único Pico com framework Arduino que conecta diretamente ao WiFi e envia leituras para a API, sem LoRa. Sensores:

- **DHT11** no GPIO 15 — temperatura (0–50 °C) e umidade do ar (20–80%).
- **HW-080** capacitivo — DO (digital, GPIO 20) para detecção limiar; AO (analógico, GPIO 28/ADC2) para percentual de umidade do solo (calibrado por `SOIL_DRY_RAW` / `SOIL_WET_RAW`).
- **LCD 16×2 I2C** (endereço 0x27 ou 0x3F) para feedback visual sem precisar de monitor serial.

O dispositivo tem um **captive portal** de provisionamento: na primeira inicialização sobe um AP (`AquaSense-XXXX`), o usuário acessa `192.168.4.1` para configurar SSID, senha e `device_id`, que são persistidos em LittleFS. Reinicializações seguintes conectam direto.

## Consequências

**Positivas**
- Validou o contrato da API (`POST /readings`) com hardware real antes de implementar LoRa.
- Confirmou que o HW-080 produz leituras estáveis com a calibração `SOIL_DRY_RAW = 950` / `SOIL_WET_RAW = 200`.
- O captive portal tornou o dispositivo autocontido — qualquer pessoa consegue configurar sem terminal.
- O LCD permite verificar leituras e status HTTP em campo sem computador.
- Acelerou o desenvolvimento do servidor e do dashboard, que passaram a ter dados reais chegando.

**Negativas**
- O `pico-dht` não representa a arquitetura final: usa WiFi direto, DHT11 (não os sensores capacitivos finais) e apenas 1 sensor de umidade de solo.
- O endereço da API está hardcoded em `main.cpp` (`http://192.168.3.8:3000/readings`) — precisa ser ajustado para cada ambiente.
- DHT11 tem precisão limitada (±2 °C, ±5% UR) — adequado para protótipo, não para produção.

## Alternativas consideradas

- **Pular o protótipo e ir direto para LoRa** — descartado: arriscado, pois problemas de API ou sensor seriam descobertos mais tarde com mais código para alterar.
- **Simulação Wokwi apenas** — descartado: útil para lógica de software, mas não valida o comportamento real dos sensores físicos nem latência de rede WiFi.
- **Usar o Pico RX (pico-rx) como protótipo WiFi** — descartado: o `pico-rx` tem sensores diferentes (ADC capacitivos, sem DHT11) e o framework Arduino não é o alvo dele.
