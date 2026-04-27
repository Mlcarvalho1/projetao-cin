import { Module } from '@nestjs/common';

import { IrrigateController } from './irrigate.controller';

@Module({
  controllers: [IrrigateController],
})
export class IrrigateModule {}
