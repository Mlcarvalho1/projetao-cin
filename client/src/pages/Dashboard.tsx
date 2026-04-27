import { useEffect, useState } from 'react';

import {
  getDashboard,
  getHealth,
  getIrrigate,
  type DashboardResponse,
  type HealthResponse,
  type IrrigateResponse,
} from '../lib/api';

type AsyncState<T> =
  | { kind: 'loading' }
  | { kind: 'ok'; data: T }
  | { kind: 'error'; message: string };

function useAsync<T>(fn: () => Promise<T>): AsyncState<T> {
  const [state, setState] = useState<AsyncState<T>>({ kind: 'loading' });
  useEffect(() => {
    let cancelled = false;
    fn()
      .then((data) => {
        if (!cancelled) setState({ kind: 'ok', data });
      })
      .catch((err: unknown) => {
        if (cancelled) return;
        const message = err instanceof Error ? err.message : 'Erro desconhecido';
        setState({ kind: 'error', message });
      });
    return () => {
      cancelled = true;
    };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);
  return state;
}

const fmtPct = (v: number) => `${(v * 100).toFixed(0)}%`;
const fmtDate = (iso: string) => new Date(iso).toLocaleString('pt-BR');

export function Dashboard() {
  const health = useAsync<HealthResponse>(getHealth);
  const dashboard = useAsync<DashboardResponse>(getDashboard);
  const irrigate = useAsync<IrrigateResponse>(getIrrigate);

  return (
    <div className="container">
      <h1 style={{ fontSize: '2rem', marginBottom: '2rem' }}>Dashboard</h1>

      <div className="card">
        <h2>Status do servidor</h2>
        {health.kind === 'loading' && (
          <p className="muted">
            <span className="status-dot status-loading" />
            Consultando /api/health…
          </p>
        )}
        {health.kind === 'error' && (
          <>
            <p>
              <span className="status-dot status-err" />
              <strong>Servidor indisponível</strong>
            </p>
            <p className="muted" style={{ marginTop: '0.5rem' }}>
              {health.message}. Verifique se o backend está rodando em{' '}
              <code>localhost:3000</code>.
            </p>
          </>
        )}
        {health.kind === 'ok' && (
          <>
            <p style={{ marginBottom: '1rem' }}>
              <span className="status-dot status-ok" />
              <strong>Online</strong>
            </p>
            <dl className="kv">
              <dt>Status</dt>
              <dd>{health.data.status}</dd>
              <dt>Ambiente</dt>
              <dd>{health.data.env}</dd>
              <dt>Uptime</dt>
              <dd>{health.data.uptime.toFixed(1)} s</dd>
              <dt>Timestamp</dt>
              <dd>{fmtDate(health.data.timestamp)}</dd>
            </dl>
          </>
        )}
      </div>

      <div className="card">
        <h2>Recomendação de irrigação</h2>
        {irrigate.kind === 'loading' && (
          <p className="muted">
            <span className="status-dot status-loading" />
            Calculando…
          </p>
        )}
        {irrigate.kind === 'error' && (
          <p className="muted">
            <span className="status-dot status-err" />
            {irrigate.message}
          </p>
        )}
        {irrigate.kind === 'ok' && (
          <>
            <p style={{ marginBottom: '1rem' }}>
              <span
                className={`status-dot ${
                  irrigate.data.shouldIrrigate ? 'status-ok' : 'status-loading'
                }`}
              />
              <strong>
                {irrigate.data.shouldIrrigate
                  ? 'Irrigar agora'
                  : 'Não irrigar agora'}
              </strong>
              <span className="muted" style={{ marginLeft: '0.5rem' }}>
                · confiança {fmtPct(irrigate.data.confidence)}
              </span>
            </p>
            <p style={{ marginBottom: '1rem' }}>{irrigate.data.reason}</p>
            <dl className="kv">
              <dt>Duração</dt>
              <dd>{irrigate.data.recommendation.durationMin} min</dd>
              <dt>Volume</dt>
              <dd>{irrigate.data.recommendation.volumeLiters} L</dd>
              <dt>Janela</dt>
              <dd>
                {fmtDate(irrigate.data.recommendation.windowStart)} —{' '}
                {fmtDate(irrigate.data.recommendation.windowEnd)}
              </dd>
              <dt>Próxima checagem</dt>
              <dd>{fmtDate(irrigate.data.nextCheckAt)}</dd>
            </dl>
          </>
        )}
      </div>

      <div className="card">
        <h2>Resumo do campo</h2>
        {dashboard.kind === 'loading' && (
          <p className="muted">
            <span className="status-dot status-loading" />
            Carregando…
          </p>
        )}
        {dashboard.kind === 'error' && (
          <p className="muted">
            <span className="status-dot status-err" />
            {dashboard.message}
          </p>
        )}
        {dashboard.kind === 'ok' && (
          <dl className="kv">
            <dt>Umidade do solo</dt>
            <dd>{fmtPct(dashboard.data.summary.soilMoisture)}</dd>
            <dt>Sensores ativos</dt>
            <dd>
              {dashboard.data.summary.activeSensors} /{' '}
              {dashboard.data.summary.totalSensors}
            </dd>
            <dt>Última leitura</dt>
            <dd>{fmtDate(dashboard.data.summary.lastReadingAt)}</dd>
            <dt>Temperatura</dt>
            <dd>{dashboard.data.weather.tempC.toFixed(1)} °C</dd>
            <dt>Umidade do ar</dt>
            <dd>{fmtPct(dashboard.data.weather.humidity)}</dd>
            <dt>Chuva (24h)</dt>
            <dd>{dashboard.data.weather.rainLast24hMm} mm</dd>
            <dt>ETo</dt>
            <dd>{dashboard.data.weather.etoMmDay} mm/dia</dd>
          </dl>
        )}
      </div>

      <div className="card">
        <h2>Alertas</h2>
        {dashboard.kind === 'ok' && dashboard.data.alerts.length === 0 && (
          <div className="empty">Sem alertas no momento.</div>
        )}
        {dashboard.kind === 'ok' &&
          dashboard.data.alerts.map((a) => (
            <p key={a.id} style={{ marginBottom: '0.5rem' }}>
              <span
                className={`status-dot ${
                  a.level === 'error'
                    ? 'status-err'
                    : a.level === 'warn'
                      ? 'status-loading'
                      : 'status-ok'
                }`}
              />
              <strong>{a.message}</strong>
              <span className="muted" style={{ marginLeft: '0.5rem' }}>
                {fmtDate(a.at)}
              </span>
            </p>
          ))}
      </div>

      <div className="card">
        <h2>Leituras recentes</h2>
        {dashboard.kind !== 'ok' && (
          <div className="empty">Carregando leituras…</div>
        )}
        {dashboard.kind === 'ok' &&
          dashboard.data.recentReadings.length === 0 && (
            <div className="empty">Nenhuma leitura disponível.</div>
          )}
        {dashboard.kind === 'ok' &&
          dashboard.data.recentReadings.length > 0 && (
            <dl className="kv">
              {dashboard.data.recentReadings.map((r, i) => (
                <span key={`${r.sensorId}-${i}`} style={{ display: 'contents' }}>
                  <dt>{fmtDate(r.at)}</dt>
                  <dd>
                    {r.sensorId} · {fmtPct(r.moisture)}
                  </dd>
                </span>
              ))}
            </dl>
          )}
      </div>
    </div>
  );
}
