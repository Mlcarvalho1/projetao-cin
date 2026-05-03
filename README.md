# AquaSense

Sistema IoT de monitoramento de irrigação de baixo custo para o produtor de médio porte do Vale do São Francisco. Sensores capacitivos no solo enviam leituras via LoRa até um receptor com WiFi, que repassa para uma API. O produtor recebe alertas e recomendações prescritivas via WhatsApp ("irrigue X mm hoje às 6h").

Projeto da disciplina **Projetão (CIn-UFPE)**. Conceito e contexto de mercado em [`/lps/index.html`](./lps/index.html).

## Estrutura do repositório

```
.
├── hardware/   Firmware C++/PlatformIO (Pico TX e RX) + protótipo pico-dht
├── server/     API REST (NestJS/TypeScript) + SQLite (MVP) + integração WhatsApp (planejado)
├── client/     Dashboard web em tempo real (React 19 + Vite)
├── lps/        Landing page de apresentação do projeto (Quest 1–3)
└── docs/       Arquitetura, decisões (ADRs), glossário, contratos de API
```

## Arquitetura em uma linha

`Sensores ADC → Pico TX → LoRa 915MHz → Pico RX (WiFi) → HTTPS → API → SQLite → Dashboard / WhatsApp`

Detalhes em [`docs/architecture.md`](./docs/architecture.md).

## Stack principal

| Camada | Tecnologia |
|---|---|
| Firmware | C++17, PlatformIO, Pico SDK |
| Protótipo v1 | Arduino framework, DHT11, HW-080, LCD I2C |
| Backend | NestJS 11, TypeScript strict, better-sqlite3 |
| Frontend | React 19, Vite 8, TypeScript |
| Banco (MVP) | SQLite (migração para PostgreSQL planejada) |

## Como rodar

**Pré-requisitos:** Node.js 20+, pnpm 9+.

```bash
# 1. Instalar dependências
pnpm install

# 2. Subir servidor e cliente juntos
pnpm dev
```

Isso inicia:
- API REST em `http://localhost:3000` (NestJS, hot-reload)
- Dashboard em `http://localhost:5173` (Vite, HMR)

O banco SQLite (`server/aquasense.db`) é criado automaticamente na primeira inicialização.

### Rodar separadamente

```bash
pnpm --filter server start:dev   # só o backend
pnpm --filter client dev         # só o frontend
```

### Enviar uma leitura de teste

```bash
curl -X POST http://localhost:3000/readings \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "pico-dht-01",
    "humidity": 65.0,
    "temperature_c": 28.5,
    "soil_wet": false,
    "soil_moisture_pct": 32,
    "ts": "2026-05-03T12:00:00Z"
  }'
```

### Firmware (protótipo pico-dht)

**Pré-requisitos:** PlatformIO Core (`pip install platformio`).

```bash
cd hardware/firmware/pico-dht

# Compilar
pio run

# Compilar e gravar (Pico em modo BOOTSEL)
pio run -t upload

# Monitor serial
pio device monitor -b 115200
```

Edite `API_URL` em `src/main.cpp` para apontar para o IP da sua máquina antes de gravar.

## Status

MVP em desenvolvimento. Protótipo `pico-dht` funcional (WiFi direto). Etapas 2–9 em andamento — veja [`docs/decisions/`](./docs/decisions/) para o histórico de decisões.
