#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

// Masukkan kredensial WiFi Anda
const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

void setup() {
  // Inisialisasi kecepatan Serial Monitor sesuai standar ESP
  Serial.begin(115200);
  delay(10);

  Serial.println('\n');
  Serial.print("Menghubungkan ke: ");
  Serial.println(ssid);

  // Memulai proses koneksi ke Access Point
  WiFi.begin(ssid, password);

  // Menahan eksekusi program (blocking) hingga status berubah menjadi WL_CONNECTED
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Jika otentikasi dan request DHCP berhasil
  Serial.println("\nKoneksi WiFi Berhasil!");
  Serial.print("IP Address ESP Anda (DHCP): ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Tidak ada instruksi berulang, IP tetap statis selama sesi koneksi aktif
}