import { Module } from '@nestjs/common';

import { ReadingsModule } from '../readings/readings.module';
import { DashboardController } from './dashboard.controller';

@Module({
  imports: [ReadingsModule],
  controllers: [DashboardController],
})
export class DashboardModule {}
