import { Injectable } from '@nestjs/common';
import { DatabaseService } from '../database/database.service';
import { CreateReadingDto } from './dto/create-reading.dto';

export interface ReadingRecord extends CreateReadingDto {
  id: number;
  received_at: string;
}

@Injectable()
export class ReadingsService {
  constructor(private readonly db: DatabaseService) {}

  add(dto: CreateReadingDto): ReadingRecord {
    const received_at = new Date().toISOString();
    const stmt = this.db.instance.prepare(`
      INSERT INTO readings
        (device_id, humidity, temperature_c, soil_wet, soil_moisture_pct, ts, received_at)
      VALUES
        (@device_id, @humidity, @temperature_c, @soil_wet, @soil_moisture_pct, @ts, @received_at)
    `);
    const result = stmt.run({
      ...dto,
      soil_wet: dto.soil_wet ? 1 : 0,
      received_at,
    });
    return { ...dto, id: result.lastInsertRowid as number, received_at };
  }

  latest(): ReadingRecord | undefined {
    const row = this.db.instance
      .prepare(
        'SELECT * FROM readings ORDER BY id DESC LIMIT 1',
      )
      .get() as ReadingRecord | undefined;
    return row ? this.deserialize(row) : undefined;
  }

  recent(n = 20): ReadingRecord[] {
    const rows = this.db.instance
      .prepare(
        'SELECT * FROM readings ORDER BY id DESC LIMIT ?',
      )
      .all(n) as ReadingRecord[];
    return rows.map((r) => this.deserialize(r));
  }

  private deserialize(row: ReadingRecord): ReadingRecord {
    return { ...row, soil_wet: Boolean(row.soil_wet) };
  }
}
