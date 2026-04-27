export type HealthResponse = {
  status: string;
  uptime: number;
  env: string;
  timestamp: string;
};

export type DashboardResponse = {
  summary: {
    soilMoisture: number;
    soilMoistureUnit: string;
    lastReadingAt: string;
    activeSensors: number;
    totalSensors: number;
  };
  weather: {
    tempC: number;
    humidity: number;
    rainLast24hMm: number;
    etoMmDay: number;
  };
  alerts: Array<{
    id: string;
    level: 'info' | 'warn' | 'error';
    message: string;
    at: string;
  }>;
  recentReadings: Array<{
    at: string;
    sensorId: string;
    moisture: number;
  }>;
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
