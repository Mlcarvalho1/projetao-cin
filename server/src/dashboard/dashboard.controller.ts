import { Controller, Get } from '@nestjs/common';

@Controller('dashboard')
export class DashboardController {
  @Get()
  getDashboard() {
    return {
      summary: {
        soilMoisture: 0.32,
        soilMoistureUnit: 'vwc',
        lastReadingAt: '2026-04-27T14:23:00.000Z',
        activeSensors: 4,
        totalSensors: 5,
      },
      weather: {
        tempC: 31.5,
        humidity: 0.42,
        rainLast24hMm: 0,
        etoMmDay: 6.2,
      },
      alerts: [
        {
          id: 'a1',
          level: 'warn',
          message: 'Setor 2 abaixo de 25% de umidade',
          at: '2026-04-27T13:45:00.000Z',
        },
      ],
      recentReadings: [
        { at: '2026-04-27T14:23:00.000Z', sensorId: 's-01', moisture: 0.3 },
        { at: '2026-04-27T14:08:00.000Z', sensorId: 's-01', moisture: 0.31 },
        { at: '2026-04-27T13:53:00.000Z', sensorId: 's-01', moisture: 0.33 },
        { at: '2026-04-27T13:38:00.000Z', sensorId: 's-02', moisture: 0.28 },
      ],
    };
  }
}
