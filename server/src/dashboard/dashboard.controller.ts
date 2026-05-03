import { Controller, Get } from '@nestjs/common';
import { ReadingsService } from '../readings/readings.service';

@Controller('dashboard')
export class DashboardController {
  constructor(private readonly readings: ReadingsService) {}

  @Get()
  getDashboard() {
    const latest = this.readings.latest();
    const recent = this.readings.recent(20);

    return {
      latest: latest ?? null,
      recent,
    };
  }
}
