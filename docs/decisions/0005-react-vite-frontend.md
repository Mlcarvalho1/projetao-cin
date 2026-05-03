# ADR 0005 — React + Vite para o frontend

- **Status:** aceito
- **Data:** 2026-05-03

## Contexto

O dashboard precisa exibir leituras de sensores em tempo real (temperatura, umidade, umidade do solo), com atualização periódica, estados de carregamento/erro, e uma UX clara para o produtor rural. O sistema roda localmente na rede da propriedade — não é uma SPA pública com requisitos de SEO ou SSR.

## Decisão

Usar **React 19** + **Vite 8** + **TypeScript strict**, sem biblioteca de estado global nem de gráficos por enquanto. O Vite serve como bundler de desenvolvimento (HMR instantâneo) e de produção. A comunicação com o backend é feita via `fetch` nativo com polling a cada 30 s — sem WebSocket por enquanto. O Vite proxy (`/api → http://localhost:3000`) elimina problemas de CORS no desenvolvimento.

## Consequências

**Positivas**
- HMR do Vite 8 (Oxc) é extremamente rápido — ciclo de iteração no dashboard em milissegundos.
- React 19 com `useCallback` e `useEffect` é suficiente para o padrão de polling — sem biblioteca de estado adicional.
- TypeScript strict compartilha os mesmos tipos do backend (ex: `Reading`) sem precisar de geração de código.
- Zero dependências de runtime além de `react`, `react-dom` e `react-router-dom`.

**Negativas**
- Polling a cada 30 s é menos responsivo que WebSocket/SSE — aceitável para dados de sensor com intervalo de 30 s.
- Sem biblioteca de gráficos ainda: os dados históricos são exibidos em tabela. Recharts ou Chart.js serão necessários para visualização temporal.
- Sem cache de dados entre navegações (cada mount refaz o fetch).

## Alternativas consideradas

- **Next.js** — descartado: SSR e file-based routing são desnecessários para um dashboard local; adiciona complexidade de deployment.
- **Vue 3 + Vite** — descartado: preferência do time por React/TypeScript.
- **Svelte** — descartado: ecossistema menor, menos familiaridade no time.
- **WebSocket em vez de polling** — adiado: polling é suficiente para 1 leitura a cada 30 s e mais simples de implementar no MVP; WebSocket pode ser adicionado quando o intervalo de atualização precisar ser menor que 5 s.
