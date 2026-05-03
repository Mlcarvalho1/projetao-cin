import {
  Body,
  Controller,
  Get,
  HttpCode,
  Ip,
  Logger,
  NotFoundException,
  Post,
} from '@nestjs/common';

import { CreateReadingDto } from './dto/create-reading.dto';
import { ReadingsService } from './readings.service';

@Controller('readings')
export class ReadingsController {
  private readonly logger = new Logger(ReadingsController.name);

  constructor(private readonly svc: ReadingsService) {}

  @Post()
  @HttpCode(201)
  create(@Body() dto: CreateReadingDto, @Ip() ip: string) {
    this.logger.log(
      `POST /readings from ${ip} — body: ${JSON.stringify(dto)}`,
    );
    const record = this.svc.add(dto);
    this.logger.log(
      `Saved: device=${record.device_id} humidity=${record.humidity.toFixed(1)}% temp=${record.temperature_c.toFixed(1)}°C ts=${record.ts}`,
    );
    return record;
  }

  @Get('latest')
  getLatest() {
    const record = this.svc.latest();
    if (!record) throw new NotFoundException('No readings yet');
    return record;
  }

  @Get()
  getRecent() {
    return this.svc.recent();
  }
}
