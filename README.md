# AquaSense

Sistema IoT de monitoramento de irrigação de baixo custo para o produtor de médio porte do Vale do São Francisco. Sensores capacitivos no solo enviam leituras via LoRa até um receptor com WiFi, que repassa para uma API. O produtor recebe alertas e recomendações prescritivas via WhatsApp ("irrigue X mm hoje às 6h").

Projeto da disciplina **Projetão (CIn-UFPE)**. Conceito e contexto de mercado em [`/lps/index.html`](./lps/index.html).

## Estrutura do repositório

```
.
├── hardware/   Firmware MicroPython (Pico TX e RX) + esquemas e BOM
├── server/     API REST (Node/NestJS) + Postgres + integração WhatsApp
├── client/     Dashboard web (gráficos em tempo real)
├── lps/        Landing page de apresentação do projeto (Quest 1–3)
└── docs/       Arquitetura, decisões (ADRs), glossário, contratos de API
```

## Arquitetura em uma linha

`Sensores ADC → Pico TX → LoRa 915MHz → Pico RX (WiFi) → HTTPS → API → Postgres → Dashboard / WhatsApp`

Detalhes em [`docs/architecture.md`](./docs/architecture.md).

## Status

Em planejamento. Acompanhe os passos de implementação e decisões em [`docs/decisions/`](./docs/decisions/).
