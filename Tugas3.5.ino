int nilai = 0; // Variabel awal yang akan dikonversi

void setup() {
  // Inisialisasi komunikasi serial dengan baud rate 115200 
  // (Standar yang optimal untuk ESP8266/ESP32)
  Serial.begin(115200);
  
  // Tunggu sejenak agar Serial Monitor siap
  delay(1000); 
  
  Serial.println("===================================================");
  Serial.println("Konversi Nilai Desimal ke Biner, Hexa, dan Oktal");
  Serial.println("===================================================");
}

void loop() {
  Serial.print("Desimal: ");
  Serial.print(nilai);        // Menampilkan nilai dalam desimal (default)

  Serial.print(" \t| Biner: ");
  Serial.print(nilai, BIN);   // Mengonversi dan menampilkan dalam Biner

  Serial.print(" \t| Hexa: ");
  Serial.print(nilai, HEX);   // Mengonversi dan menampilkan dalam Heksadesimal

  Serial.print(" \t| Oktal: ");
  Serial.println(nilai, OCT); // Mengonversi dan menampilkan dalam Oktal

  nilai++; // Menambah nilai sebesar 1 setiap iterasi

  delay(1000); // Jeda 1 detik agar data mudah dibaca di Serial Monitor

  // Opsional: Mereset nilai kembali ke 0 jika sudah mencapai 255 (batas 8-bit)
  if (nilai > 255) {
    nilai = 0;
    Serial.println("------------------ RESET KE 0 ------------------");
    delay(2000);
  }
}