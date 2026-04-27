# ADR 0001 — LoRa no lugar de UART entre Campo e Receptor

- **Status:** aceito
- **Data:** 2026-04-27

## Contexto

A arquitetura de referência conecta o nó de campo (Pico TX, com 3 sensores de umidade) ao receptor (Pico RX + WiFi) via **UART a 9600 baud**. UART exige cabo TX/RX/GND ligando os dois dispositivos. Em uma propriedade real do Vale do São Francisco, o ponto bom para sensoriamento (na zona radicular, no meio da plantação) fica facilmente a 100–500 m da casa, onde estão a tomada e o WiFi. Cabear essa distância é caro, frágil (sol, gado, máquina agrícola) e inviável em propriedades de 12+ hectares.

## Decisão

Substituir o link UART por **LoRa em 915 MHz** (faixa ISM livre no Brasil). O nó de campo passa a ser totalmente sem fio, alimentado por bateria + painel solar.

## Consequências

**Positivas**
- Alcance de 300 m a 2 km sem cabo, dependendo de SF e linha de visão.
- Nó de campo realmente autônomo (deepsleep entre leituras → dura meses por carga).
- Possível adicionar mais nós futuramente sem refazer cabeamento.

**Negativas**
- Custo por nó sobe: módulo LoRa (~R$ 30–60) + antena.
- Payload precisa ser binário compacto (< 32 B) e protegido por CRC + HMAC.
- Throughput muito menor que UART — ok para 1 leitura a cada 30 s, ruim para qualquer streaming.
- Requer atenção a duty cycle e legislação Anatel (Res. 680).

## Alternativas consideradas

- **UART original** — descartado pelo problema do cabo.
- **WiFi direto no nó de campo** — descartado: WiFi não chega na lavoura, e mesmo se chegasse, consumo inviabiliza operação por bateria.
- **NB-IoT / LTE-M** — descartado para o MVP: depende de cobertura de operadora e exige chip + plano de dados por nó.
- **Zigbee** — alcance insuficiente sem mesh, e mesh adiciona complexidade fora do escopo do MVP.
