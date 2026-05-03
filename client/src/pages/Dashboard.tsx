import { useCallback, useEffect, useRef, useState } from 'react';

import {
  getDashboard,
  getHealth,
  type DashboardResponse,
  type HealthResponse,
  type Reading,
} from '../lib/api';

const REFRESH_MS = 30_000;

function usePolled<T>(fn: () => Promise<T>, intervalMs: number) {
  const [data, setData] = useState<T | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(true);
  const [lastFetched, setLastFetched] = useState<Date | null>(null);
  const fnRef = useRef(fn);
  fnRef.current = fn;

  const fetch = useCallback(() => {
    fnRef
      .current()
      .then((d) => {
        setData(d);
        setError(null);
        setLastFetched(new Date());
      })
      .catch((e: unknown) =>
        setError(e instanceof Error ? e.message : 'Erro desconhecido'),
      )
      .finally(() => setLoading(false));
  }, []);

  useEffect(() => {
    fetch();
    const id = setInterval(fetch, intervalMs);
    return () => clearInterval(id);
  }, [fetch, intervalMs]);

  return { data, error, loading, lastFetched, refresh: fetch };
}

function fmtDate(iso: string) {
  return new Date(iso).toLocaleString('pt-BR', {
    day: '2-digit',
    month: '2-digit',
    year: '2-digit',
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit',
  });
}

function MetricCard({
  label,
  value,
  unit,
  sub,
  color,
}: {
  label: string;
  value: string | number;
  unit?: string;
  sub?: string;
  color?: string;
}) {
  return (
    <div className="metric-card" style={color ? { borderTopColor: color } : {}}>
      <div className="metric-label">{label}</div>
      <div className="metric-value">
        {value}
        {unit && <span className="metric-unit">{unit}</span>}
      </div>
      {sub && <div className="metric-sub">{sub}</div>}
    </div>
  );
}

function LatestReadingPanel({ r }: { r: Reading }) {
  return (
    <>
      <div className="metrics-grid">
        <MetricCard
          label="Temperatura"
          value={r.temperature_c.toFixed(1)}
          unit="°C"
          color="var(--accent)"
        />
        <MetricCard
          label="Umidade do Ar"
          value={r.humidity.toFixed(1)}
          unit="%"
          color="var(--water)"
        />
        <MetricCard
          label="Umidade do Solo"
          value={r.soil_moisture_pct}
          unit="%"
          color="var(--leaf)"
        />
        <MetricCard
          label="Solo"
          value={r.soil_wet ? 'Úmido' : 'Seco'}
          sub={r.soil_wet ? 'sensor ativo' : 'abaixo do limiar'}
          color={r.soil_wet ? 'var(--water-deep)' : 'var(--sand)'}
        />
      </div>
      <div className="reading-meta">
        <span className="muted">Dispositivo</span>
        <strong>{r.device_id}</strong>
        <span className="muted" style={{ marginLeft: '1.5rem' }}>
          Leitura do sensor
        </span>
        <strong>{fmtDate(r.ts)}</strong>
        <span className="muted" style={{ marginLeft: '1.5rem' }}>
          Recebido em
        </span>
        <strong>{fmtDate(r.received_at)}</strong>
      </div>
    </>
  );
}

function ReadingsTable({ rows }: { rows: Reading[] }) {
  if (rows.length === 0) {
    return (
      <div className="empty">Nenhuma leitura disponível. Aguardando dados do sensor…</div>
    );
  }
  return (
    <div className="table-wrap">
      <table className="readings-table">
        <thead>
          <tr>
            <th>#</th>
            <th>Dispositivo</th>
            <th>Temp (°C)</th>
            <th>Umidade (%)</th>
            <th>Solo (%)</th>
            <th>Solo</th>
            <th>Timestamp</th>
          </tr>
        </thead>
        <tbody>
          {rows.map((r) => (
            <tr key={r.id}>
              <td className="muted">{r.id}</td>
              <td>
                <code>{r.device_id}</code>
              </td>
              <td>{r.temperature_c.toFixed(1)}</td>
              <td>{r.humidity.toFixed(1)}</td>
              <td>
                <div className="moisture-bar-wrap">
                  <div
                    className="moisture-bar"
                    style={{ width: `${r.soil_moisture_pct}%` }}
                  />
                  <span>{r.soil_moisture_pct}%</span>
                </div>
              </td>
              <td>
                <span className={`badge ${r.soil_wet ? 'badge-wet' : 'badge-dry'}`}>
                  {r.soil_wet ? 'Úmido' : 'Seco'}
                </span>
              </td>
              <td className="muted">{fmtDate(r.ts)}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

function HealthBadge({ h }: { h: HealthResponse }) {
  return (
    <span className="health-badge">
      <span className="status-dot status-ok" />
      {h.status} · uptime {h.uptime.toFixed(0)}s · {h.env}
    </span>
  );
}

export function Dashboard() {
  const health = usePolled<HealthResponse>(getHealth, REFRESH_MS);
  const dashboard = usePolled<DashboardResponse>(getDashboard, REFRESH_MS);

  const hasData = dashboard.data && dashboard.data.recent.length > 0;

  return (
    <div className="container">
      <div className="dash-header">
        <div>
          <h1>AquaSense</h1>
          <p className="muted" style={{ marginTop: '0.25rem' }}>
            Monitoramento em tempo real do solo e ambiente
          </p>
        </div>
        <div className="dash-header-right">
          {health.data && <HealthBadge h={health.data} />}
          {dashboard.lastFetched && (
            <button className="btn-refresh" onClick={dashboard.refresh}>
              ↻ Atualizar
            </button>
          )}
        </div>
      </div>

      {dashboard.error && (
        <div className="alert-banner alert-error">
          <span className="status-dot status-err" />
          Falha ao carregar dados: {dashboard.error}
        </div>
      )}

      {dashboard.loading && !dashboard.data && (
        <div className="alert-banner">
          <span className="status-dot status-loading" />
          Carregando leituras…
        </div>
      )}

      <div className="card">
        <h2>Última Leitura</h2>
        {!hasData ? (
          <div className="empty">
            Nenhuma leitura ainda. Aguardando o primeiro envio do sensor.
          </div>
        ) : (
          <LatestReadingPanel r={dashboard.data!.latest!} />
        )}
      </div>

      <div className="card">
        <div className="card-header-row">
          <h2>Histórico de Leituras</h2>
          {dashboard.lastFetched && (
            <span className="muted" style={{ fontSize: '0.8rem' }}>
              Atualizado em {fmtDate(dashboard.lastFetched.toISOString())} · a cada 30s
            </span>
          )}
        </div>
        {dashboard.loading && !dashboard.data ? (
          <div className="empty">Carregando…</div>
        ) : (
          <ReadingsTable rows={dashboard.data?.recent ?? []} />
        )}
      </div>
    </div>
  );
}
