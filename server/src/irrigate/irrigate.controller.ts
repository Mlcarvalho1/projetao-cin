import { Controller, Get } from '@nestjs/common';

@Controller('irrigate')
export class IrrigateController {
  @Get()
  getIrrigate() {
    return {
      shouldIrrigate: true,
      confidence: 0.78,
      reason:
        'Umidade do solo abaixo do limiar crítico (28%) e sem previsão de chuva nas próximas 24h.',
      recommendation: {
        durationMin: 35,
        volumeLiters: 1200,
        windowStart: '2026-04-27T17:00:00.000Z',
        windowEnd: '2026-04-27T19:00:00.000Z',
      },
      nextCheckAt: '2026-04-27T20:00:00.000Z',
    };
  }
}
