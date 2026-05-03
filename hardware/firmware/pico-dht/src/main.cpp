// AquaSense pico-dht v1 — DHT11 on GPIO15, WiFi via captive portal
//
// First boot:  creates AP "AquaSense-XXXX", open http://192.168.4.1 to configure
// Normal boot: loads config from flash, connects WiFi, POSTs every 30 s
// Reset config: delete /cfg.txt via portal or reflash

#include <Arduino.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>

constexpr uint8_t  DHT_PIN     = 15;
constexpr uint32_t INTERVAL_MS = 30'000;
constexpr const char* API_URL  = "http://192.168.3.6:3000/readings";

LiquidCrystal_I2C lcd(0x27,16,2);


// ─── Config ───────────────────────────────────────────────────────────────

struct Config {
    char ssid[64]      = "";
    char pass[64]      = "";
    char device_id[32] = "pico-dht-01";
};

static Config cfg;
static bool   cfg_valid = false;

static void loadConfig() {
    if (!LittleFS.begin()) { Serial.println("[fs] mount failed"); return; }
    File f = LittleFS.open("/cfg.txt", "r");
    if (!f) return;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        int sep = line.indexOf('=');
        if (sep < 0) continue;
        String k = line.substring(0, sep);
        String v = line.substring(sep + 1);
        if      (k == "ssid")      v.toCharArray(cfg.ssid,      sizeof(cfg.ssid));
        else if (k == "pass")      v.toCharArray(cfg.pass,      sizeof(cfg.pass));
        else if (k == "device_id") v.toCharArray(cfg.device_id, sizeof(cfg.device_id));
    }
    f.close();
    cfg_valid = (cfg.ssid[0] != '\0');
    if (cfg_valid)
        Serial.printf("[cfg] id=%s  url=%s\n", cfg.device_id, API_URL);
}

static void persistConfig(const String& ssid, const String& pass,
                          const String& device_id) {
    LittleFS.begin();
    File f = LittleFS.open("/cfg.txt", "w");
    if (!f) { Serial.println("[fs] write failed"); return; }
    f.printf("ssid=%s\n",      ssid.c_str());
    f.printf("pass=%s\n",      pass.c_str());
    f.printf("device_id=%s\n", device_id.c_str());
    f.close();
    Serial.println("[cfg] saved");
}

// ─── Captive portal ───────────────────────────────────────────────────────

static const char PORTAL_HTML[] = R"html(
<!DOCTYPE html><html><head><meta charset=utf-8>
<meta name=viewport content="width=device-width,initial-scale=1">
<title>AquaSense Setup</title>
<style>
  body{font-family:sans-serif;max-width:400px;margin:2rem auto;padding:0 1rem}
  h2{color:#1a7a4a}
  label{display:block;font-size:.9rem;margin:.8rem 0 .2rem}
  input{width:100%;box-sizing:border-box;padding:.5rem;border:1px solid #ccc;border-radius:4px}
  button{margin-top:1.4rem;width:100%;padding:.7rem;background:#1a7a4a;color:#fff;
         border:none;border-radius:4px;cursor:pointer;font-size:1rem}
</style></head><body>
<h2>AquaSense Setup</h2>
<form method=POST action=/save>
  <label>WiFi SSID</label>
  <input name=ssid required>
  <label>WiFi Password</label>
  <input name=pass type=password>
  <label>Device ID</label>
  <input name=device_id value="pico-dht-01" required>
  <button type=submit>Salvar e Conectar</button>
</form></body></html>
)html";

static void startPortal() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char ap_name[24];
    snprintf(ap_name, sizeof(ap_name), "AquaSense-%02X%02X", mac[4], mac[5]);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_name);
    Serial.printf("[portal] AP \"%s\"  → http://%s\n",
        ap_name, WiFi.softAPIP().toString().c_str());

    WebServer server(80);

    server.on("/", HTTP_GET, [&]() {
        server.send(200, "text/html", PORTAL_HTML);
    });

    server.on("/save", HTTP_POST, [&]() {
        String ssid      = server.arg("ssid");
        String pass      = server.arg("pass");
        String device_id = server.arg("device_id");

        if (ssid.isEmpty() || device_id.isEmpty()) {
            server.send(400, "text/plain", "ssid e device_id são obrigatórios");
            return;
        }

        persistConfig(ssid, pass, device_id);
        server.send(200, "text/html",
            "<!DOCTYPE html><html><body><h2>Salvo!</h2>"
            "<p>Reiniciando em 2 segundos...</p></body></html>");
        delay(2000);
        rp2040.restart();
    });

    // Redirect unknown paths to the form (captive portal behaviour)
    server.onNotFound([&]() {
        server.sendHeader("Location", "/");
        server.send(302);
    });

    server.begin();
    while (true) server.handleClient();
}

// ─── WiFi & NTP ───────────────────────────────────────────────────────────

static bool connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(cfg.ssid, cfg.pass);
    Serial.printf("[wifi] connecting to %s", cfg.ssid);
    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; ++i) {
        delay(1000);
        Serial.print('.');
    }
    if (WiFi.status() != WL_CONNECTED) { Serial.println(" failed"); return false; }
    Serial.printf(" ok — %s\n", WiFi.localIP().toString().c_str());
    return true;
}

static void syncNTP() {
    configTime(0, 0, "pool.ntp.org", "time.cloudflare.com");
    Serial.print("[ntp] syncing");
    time_t now = 0;
    for (int i = 0; i < 20 && now < 1'000'000'000; ++i) { delay(500); Serial.print('.'); now = time(nullptr); }
    Serial.println(now >= 1'000'000'000 ? " ok" : " failed");
}

static void isoNow(char* buf, size_t n) {
    time_t now = time(nullptr);
    if (now >= 1'000'000'000) {
        struct tm t{};
        gmtime_r(&now, &t);
        strftime(buf, n, "%Y-%m-%dT%H:%M:%SZ", &t);
    } else {
        snprintf(buf, n, "boot+%lus", millis() / 1000UL);
    }
}

// ─── Sensor + HTTP POST ───────────────────────────────────────────────────

static DHT      dht(DHT_PIN, DHT11);
static uint32_t seq = 0;

static void postReading(float humidity, float tempC) {
    char ts[32];
    isoNow(ts, sizeof(ts));

    char body[192];
    snprintf(body, sizeof(body),
        "{\"device_id\":\"%s\",\"humidity\":%.1f,\"temperature_c\":%.1f,\"ts\":\"%s\"}",
        cfg.device_id, humidity, tempC, ts);

    HTTPClient http;
    http.begin(API_URL);
    http.addHeader("Content-Type", "application/json");
    int status = http.POST(body);
    http.end();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.printf("H: %.1f%%", humidity);
    lcd.setCursor(0,1);
    lcd.printf("T: %.1fC", tempC);

    Serial.printf("[%s] seq=%u  h=%.1f%%  t=%.1f°C  ts=%s  → HTTP %d\n",
        status == 201 ? "ok" : "err", seq++, humidity, tempC, ts, status);
}

// ─── Utils ────────────────────────────────────────────────────────────────

bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

// ─── Arduino entry points ─────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\naquasense pico-dht v1 — GPIO15 DHT11");

    dht.begin();
    loadConfig();

    if (!i2CAddrTest(0x27)) {
        lcd = LiquidCrystal_I2C(0x3F, 16, 2);
    }
    lcd.setCursor(0,0);
    lcd.init();                     // LCD driver initialization
    lcd.backlight();
    lcd.print("AquaSense v1");
    lcd.setCursor(0,1);
    lcd.print("Booting...");

    if (!cfg_valid) {
        Serial.println("[boot] no config found — starting provisioning portal");
        startPortal();  // blocks until user saves config and device restarts
    }

    if (!connectWiFi()) {
        Serial.println("[boot] WiFi failed — clearing config and re-provisioning");
        LittleFS.begin();
        LittleFS.remove("/cfg.txt");
        startPortal();
    }

    syncNTP();
    delay(2000);  // DHT settling time after power-on
}

void loop() {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Read error");
        Serial.println("[dht] read error (checksum/timeout) — retrying next cycle");
    } else if (h < 0.0f || h > 100.0f || t < 0.0f || t > 50.0f) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sensor error");
        Serial.printf("[dht] out-of-range h=%.1f t=%.1f — wiring issue?\n", h, t);
    } else {
        postReading(h, t);
    }

    delay(INTERVAL_MS);
}
