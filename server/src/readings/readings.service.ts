import { Injectable } from '@nestjs/common';
import { CreateReadingDto } from './dto/create-reading.dto';

export interface ReadingRecord extends CreateReadingDto {
  received_at: string;
}

@Injectable()
export class ReadingsService {
  private readonly store: ReadingRecord[] = [];

  add(dto: CreateReadingDto): ReadingRecord {
    const record: ReadingRecord = {
      ...dto,
      received_at: new Date().toISOString(),
    };
    this.store.push(record);
    return record;
  }

  latest(): ReadingRecord | undefined {
    return this.store.at(-1);
  }

  recent(n = 20): ReadingRecord[] {
    return this.store.slice(-n).reverse();
  }
}
