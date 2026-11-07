
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <DHT.h>

// ===================================
// KONFIGURASI WIFI WOKWI
// ===================================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ===================================
// KONFIGURASI ANTARES
// ===================================
const char* accessKey = "bd6d37d3895accdb:590829833f2d48ae";
const char* appName = "SmartEnvironmentalMonitoring";
const char* deviceName = "ESP32_Monitoring";

// Endpoint ANTARES
String serverURL;

// ===================================
// KONFIGURASI SENSOR
// ===================================
#define DHT_PIN 15
#define DHTTYPE DHT22
#define LDR_PIN 34
#define LED_PIN 2

// Inisialisasi DHT22
DHT dht(DHT_PIN, DHTTYPE);

// Interval pengiriman data
unsigned long previousMillis = 0;
const unsigned long interval = 10000;

// ===================================
// FUNGSI ESCAPE JSON
// ===================================
String escapeJSON(String data) {
  data.replace("\\", "\\\\");
  data.replace("\"", "\\\"");
  return data;
}

// ===================================
// FUNGSI KONEKSI WIFI
// ===================================
bool connectWiFi() {

  Serial.println("\nMenghubungkan ke WiFi Wokwi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password, 6);

  int retry = 0;

  while (WiFi.status() != WL_CONNECTED && retry < 60) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("\nWiFi berhasil terhubung!");

    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    return true;

  } else {

    Serial.println("\nGagal terhubung ke WiFi!");
    Serial.print("Status WiFi: ");
    Serial.println(WiFi.status());

    return false;
  }
}

// ===================================
// SETUP
// ===================================
void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("\n================================");
  Serial.println("SMART ENVIRONMENTAL MONITORING");
  Serial.println("================================");

  // Inisialisasi pin
  pinMode(LED_PIN, OUTPUT);

  // Inisialisasi sensor DHT22
  dht.begin();

  // Membuat URL ANTARES
  serverURL =
    "https://platform.antares.id:8443/~/antares-cse/antares-id/";

  serverURL += appName;
  serverURL += "/";
  serverURL += deviceName;

  // Menghubungkan WiFi
  connectWiFi();

  Serial.println("\nSistem Monitoring Dimulai!");
}

// ===================================
// LOOP
// ===================================
void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    // ===================================
    // MEMBACA SENSOR DHT22
    // ===================================

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Membaca sensor LDR
    int lightValue = analogRead(LDR_PIN);

    // Memeriksa pembacaan sensor
    if (isnan(temperature) || isnan(humidity)) {

      Serial.println("Gagal membaca sensor DHT22!");
      return;

    }

    // ===================================
    // KONTROL LED
    // ===================================

    // Kondisi gelap = LED ON
    int ledStatus = (lightValue < 1800) ? 1 : 0;

    digitalWrite(LED_PIN, ledStatus);

    // ===================================
    // MENAMPILKAN DATA SENSOR
    // ===================================

    Serial.println("\n===== DATA SENSOR =====");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Light Intensity: ");
    Serial.println(lightValue);

    Serial.print("LED Status: ");
    Serial.println(ledStatus ? "ON" : "OFF");

    // ===================================
    // MEMBUAT JSON DATA SENSOR
    // ===================================

    String sensorData = "{";

    sensorData += "\"temperature\":";
    sensorData += String(temperature, 1);
    sensorData += ",";

    sensorData += "\"humidity\":";
    sensorData += String(humidity, 1);
    sensorData += ",";

    sensorData += "\"light_intensity\":";
    sensorData += String(lightValue);
    sensorData += ",";

    sensorData += "\"led_status\":";
    sensorData += String(ledStatus);

    sensorData += "}";

    Serial.println("\nJSON Sensor:");
    Serial.println(sensorData);

    // ===================================
    // MEMBUAT PAYLOAD ANTARES
    // ===================================

    String payload = "{\"m2m:cin\":{\"con\":\"";
    payload += escapeJSON(sensorData);
    payload += "\"}}";

    // ===================================
    // MENGIRIM DATA KE ANTARES
    // ===================================

    if (WiFi.status() != WL_CONNECTED) {

      Serial.println("\nWiFi terputus!");
      Serial.println("Mencoba menghubungkan ulang...");

      if (!connectWiFi()) {
        Serial.println("Pengiriman data dibatalkan.");
        return;
      }
    }

    WiFiClientSecure client;

    // Untuk simulasi Wokwi
    client.setInsecure();

    HTTPClient http;

    Serial.println("\nMengirim data ke ANTARES...");

    if (http.begin(client, serverURL)) {

      http.addHeader("X-M2M-Origin", accessKey);
      http.addHeader("Content-Type", "application/json;ty=4");
      http.addHeader("Accept", "application/json");

      int httpResponseCode = http.POST(payload);

      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);

      if (httpResponseCode > 0) {

        Serial.println("\nResponse ANTARES:");
        Serial.println(http.getString());

        if (httpResponseCode == 201) {

          Serial.println("\nDATA BERHASIL DIKIRIM KE ANTARES!");

        } else {

          Serial.println("\nPengiriman belum berhasil.");
          Serial.println("Periksa Application, Device, dan Access Key.");

        }

      } else {

        Serial.print("HTTP Error: ");
        Serial.println(http.errorToString(httpResponseCode));

      }

      http.end();

    } else {

      Serial.println("Gagal memulai koneksi HTTP!");

    }

    Serial.println("\n========================");
  }
}
