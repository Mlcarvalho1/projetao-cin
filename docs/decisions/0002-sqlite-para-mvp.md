# ADR 0002 — SQLite para o MVP em vez de PostgreSQL

- **Status:** aceito
- **Data:** 2026-05-03

## Contexto

A arquitetura alvo usa PostgreSQL: há um `docker-compose.yml` com `postgres:16-alpine` e a variável `DATABASE_URL` já está definida no `.env.example`. O NestJS tem suporte nativo a Postgres via TypeORM ou Prisma. Porém, no MVP, a API roda localmente na máquina do desenvolvedor junto com o receptor Pico RX — não há servidor dedicado, não há DevOps, e o overhead de manter um container Postgres rodando 24h para receber 2 leituras por minuto é desnecessário.

## Decisão

Usar **SQLite** via `better-sqlite3` no MVP. O banco fica em um único arquivo (`aquasense.db`) no diretório de trabalho do servidor. Nenhum container, nenhuma string de conexão, nenhum schema migration tool por enquanto — a tabela é criada via `CREATE TABLE IF NOT EXISTS` no `onModuleInit` do `DatabaseService`.

## Consequências

**Positivas**
- Zero configuração: o banco nasce na primeira inicialização do servidor.
- `better-sqlite3` é síncrono — integração limpa com o modelo imperativo do NestJS sem precisar de async/await em cada query.
- Backup simples: copiar um arquivo.
- Suficiente para a carga do MVP (1 dispositivo, 1 leitura a cada 30 s).

**Negativas**
- Não suporta múltiplas escritas concorrentes de processos distintos (ok para MVP, problemático em produção com múltiplos workers).
- Quando migrar para Postgres, será necessário exportar os dados históricos do `.db`.
- Sem ORM: queries em SQL puro — mais frágil a refatorações de schema.

## Alternativas consideradas

- **PostgreSQL desde o início** — descartado: exige container rodando, mais configuração, e não agrega valor enquanto o sistema tem um único dispositivo físico.
- **SQLite + Prisma** — descartado para o MVP: Prisma resolve o problema de ORM e facilita a migração, mas adiciona uma camada de complexidade (geração de cliente, migrations) que atrasa o ciclo de iteração.
- **In-memory (array TypeScript)** — foi o ponto de partida, descartado porque os dados são perdidos a cada restart do servidor.

## Plano de migração

Quando o sistema for para produção (múltiplos dispositivos, servidor dedicado):

1. Adicionar Prisma com provider `postgresql`.
2. Gerar migration inicial a partir do schema atual.
3. Exportar `aquasense.db` para SQL e importar no Postgres.
4. Trocar `DatabaseService` por `PrismaService`.
