export type HealthResponse = {
  status: string;
  uptime: number;
  env: string;
  timestamp: string;
};

export type Reading = {
  id: number;
  device_id: string;
  humidity: number;
  temperature_c: number;
  soil_wet: boolean;
  soil_moisture_pct: number;
  ts: string;
  received_at: string;
};

export type DashboardResponse = {
  latest: Reading | null;
  recent: Reading[];
};

export type IrrigateResponse = {
  shouldIrrigate: boolean;
  confidence: number;
  reason: string;
  recommendation: {
    durationMin: number;
    volumeLiters: number;
    windowStart: string;
    windowEnd: string;
  };
  nextCheckAt: string;
};

async function getJson<T>(path: string): Promise<T> {
  const res = await fetch(path);
  if (!res.ok) {
    throw new Error(`Request to ${path} failed: ${res.status}`);
  }
  return res.json() as Promise<T>;
}

export function getHealth(): Promise<HealthResponse> {
  return getJson<HealthResponse>('/api/health');
}

export function getDashboard(): Promise<DashboardResponse> {
  return getJson<DashboardResponse>('/api/dashboard');
}

export function getIrrigate(): Promise<IrrigateResponse> {
  return getJson<IrrigateResponse>('/api/irrigate');
}
