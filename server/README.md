# AquaSense — Servidor

API REST do AquaSense. Recebe leituras de umidade do receptor LoRa e expõe os dados para o dashboard e para o motor de alertas WhatsApp.

Stack: **NestJS 11 · TypeScript strict · Postgres 16 · Prisma (em S2)**.

## Pré-requisitos

- Node 20+ (testado em 24)
- pnpm 9+ (testado em 10)
- Docker (para o Postgres local)

## Subir ambiente

Da raiz do repositório:

```bash
docker compose up -d postgres        # Postgres em localhost:5432
cp server/.env.example server/.env   # ajuste se necessário
cd server
pnpm install
pnpm start:dev
```

Validar:

```bash
curl http://localhost:3000/health
# { "status": "ok", "uptime": 1.23, "env": "development", "timestamp": "..." }
```

## Estrutura (atual — S1)

```
server/
├── src/
│   ├── main.ts              bootstrap + ValidationPipe global
│   ├── app.module.ts        ConfigModule (validado) + HealthModule
│   ├── config/
│   │   └── env.validation.ts
│   └── health/
│       ├── health.controller.ts
│       └── health.module.ts
├── test/
├── .env.example
├── package.json
└── tsconfig.json            strict + noUncheckedIndexedAccess
```

A cada passo (S2…S9) novos módulos são adicionados — ver [`/docs/decisions/`](../docs/decisions/) para o porquê das escolhas.

## Scripts

| Comando | O que faz |
|---|---|
| `pnpm start:dev` | dev com watch |
| `pnpm build` | build de produção em `dist/` |
| `pnpm start:prod` | roda o build |
| `pnpm test` | unit tests |
| `pnpm test:e2e` | testes e2e |
| `pnpm lint` | eslint --fix |
| `pnpm format` | prettier |

## Próximo passo

**S2 — Persistência com Prisma + Postgres.** Schema `Device` e `Reading`, primeira migration, `PrismaService` global.
