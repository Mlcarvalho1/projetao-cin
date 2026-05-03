import { Injectable, OnModuleInit } from '@nestjs/common';
import Database from 'better-sqlite3';
import * as path from 'path';

@Injectable()
export class DatabaseService implements OnModuleInit {
  private db!: Database.Database;

  onModuleInit() {
    this.db = new Database(path.resolve(process.cwd(), 'aquasense.db'));
    this.db.pragma('journal_mode = WAL');
    this.db.exec(`
      CREATE TABLE IF NOT EXISTS readings (
        id              INTEGER PRIMARY KEY AUTOINCREMENT,
        device_id       TEXT    NOT NULL,
        humidity        REAL    NOT NULL,
        temperature_c   REAL    NOT NULL,
        soil_wet        INTEGER NOT NULL,
        soil_moisture_pct INTEGER NOT NULL,
        ts              TEXT    NOT NULL,
        received_at     TEXT    NOT NULL
      )
    `);
  }

  get instance(): Database.Database {
    return this.db;
  }
}
