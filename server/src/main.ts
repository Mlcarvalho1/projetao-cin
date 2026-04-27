import { ValidationPipe } from '@nestjs/common';
import { ConfigService } from '@nestjs/config';
import { NestFactory } from '@nestjs/core';

import { AppModule } from './app.module';
import type { EnvVars } from './config/env.validation';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);

  app.useGlobalPipes(
    new ValidationPipe({
      whitelist: true,
      forbidNonWhitelisted: true,
      transform: true,
    }),
  );

  const config = app.get(ConfigService<EnvVars, true>);
  const port = config.get('PORT', { infer: true });

  await app.listen(port);
  // eslint-disable-next-line no-console
  console.log(`AquaSense API listening on http://localhost:${port}`);
}

void bootstrap();
