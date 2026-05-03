import { IsBoolean, IsNotEmpty, IsNumber, IsString, Max, Min } from 'class-validator';

export class CreateReadingDto {
  @IsString()
  @IsNotEmpty()
  device_id!: string;

  @IsNumber()
  @Min(0)
  @Max(100)
  humidity!: number;

  @IsNumber()
  @Min(-40)
  @Max(80)
  temperature_c!: number;

  @IsBoolean()
  soil_wet!: boolean;

  @IsNumber()
  @Min(0)
  @Max(100)
  soil_moisture_pct!: number;

  @IsString()
  @IsNotEmpty()
  ts!: string;
}
