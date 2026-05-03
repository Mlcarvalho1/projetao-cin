import { BadRequestException, Logger, ValidationPipe } from '@nestjs/common';
import { ConfigService } from '@nestjs/config';
import { NestFactory } from '@nestjs/core';

import { AppModule } from './app.module';
import type { EnvVars } from './config/env.validation';

async function bootstrap() {
  const app = await NestFactory.create(AppModule);
  const logger = new Logger('Bootstrap');

  app.useGlobalPipes(
    new ValidationPipe({
      whitelist: true,
      forbidNonWhitelisted: true,
      transform: true,
      exceptionFactory: (errors) => {
        logger.warn(
          `Validation failed: ${JSON.stringify(errors.map((e) => ({ field: e.property, constraints: e.constraints })))}`,
        );
        return new BadRequestException(errors);
      },
    }),
  );

  const config = app.get(ConfigService<EnvVars, true>);
  const port = config.get('PORT', { infer: true });

  await app.listen(port);
  logger.log(`AquaSense API listening on http://localhost:${port}`);
}

void bootstrap();
