# ADR 0004 — NestJS para o backend

- **Status:** aceito
- **Data:** 2026-05-03

## Contexto

O servidor precisa de uma API REST que:
- Receba leituras dos sensores via `POST /readings` (JSON, validação estrita de campos).
- Sirva dados ao dashboard via `GET /dashboard`, `GET /readings`, etc.
- Seja extensível: no futuro haverá autenticação, integração com WhatsApp, modelo de irrigação prescritiva.

O time tem familiaridade com TypeScript e Node.js. A escolha do framework afeta estrutura de código, testabilidade e facilidade de adicionar funcionalidades futuras.

## Decisão

Usar **NestJS 11** com TypeScript strict (`strict: true`, `noUncheckedIndexedAccess: true`). O NestJS fornece um container de injeção de dependência, sistema de módulos, `ValidationPipe` global com `class-validator`/`class-transformer`, e convenções claras para controllers, services e DTOs.

## Consequências

**Positivas**
- `ValidationPipe` com `whitelist: true` rejeita campos desconhecidos automaticamente — protege contra payloads malformados enviados pelo firmware.
- DI nativo facilita trocar implementações (ex: `ReadingsService` de in-memory para SQLite foi uma mudança de 1 arquivo sem tocar no controller).
- Módulos isolados (`ReadingsModule`, `DatabaseModule` global, etc.) tornam o código navegável.
- Decorators (`@Controller`, `@Get`, `@Post`, `@Body`, `@Ip`) reduzem boilerplate.
- Testabilidade: cada módulo pode ser testado isoladamente com `Test.createTestingModule`.

**Negativas**
- Mais pesado que Express puro ou Fastify: tempo de boot e bundle maior.
- Curva de aprendizado nos conceitos de módulos, providers e DI para quem não conhece.
- Overhead de decorators e metadata pode ser confuso em projetos pequenos.

## Alternativas consideradas

- **Express puro** — descartado: sem estrutura imposta, a base de código tende a crescer sem convenções claras; validação de payload exigiria setup manual.
- **Fastify** — descartado: mais rápido, mas o ecossistema de plugins é menor e a integração com `class-validator` menos idiomática.
- **Hono** — descartado: excelente para edge/serverless, mas não agrega valor num servidor local Node.
