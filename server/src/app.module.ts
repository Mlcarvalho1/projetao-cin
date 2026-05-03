import { Module } from '@nestjs/common';
import { ConfigModule } from '@nestjs/config';

import { validateEnv } from './config/env.validation';
import { DashboardModule } from './dashboard/dashboard.module';
import { HealthModule } from './health/health.module';
import { IrrigateModule } from './irrigate/irrigate.module';
import { ReadingsModule } from './readings/readings.module';

@Module({
  imports: [
    ConfigModule.forRoot({
      isGlobal: true,
      cache: true,
      validate: validateEnv,
    }),
    HealthModule,
    DashboardModule,
    IrrigateModule,
    ReadingsModule,
  ],
})
export class AppModule {}
