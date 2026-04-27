import { Controller, Get } from '@nestjs/common';
import { ConfigService } from '@nestjs/config';

import { EnvVars } from '../config/env.validation';

@Controller('health')
export class HealthController {
  constructor(private readonly config: ConfigService<EnvVars, true>) {}

  @Get()
  check() {
    return {
      status: 'ok',
      uptime: process.uptime(),
      env: this.config.get('NODE_ENV', { infer: true }),
      timestamp: new Date().toISOString(),
    };
  }
}
