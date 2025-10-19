#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ===== BURAYA KENDI BİLGİLERİNİZİ YAZIN =====
const char* ssid = "Airties_Air4960R_83FP-2.4G";           // WiFi adınız
const char* password = "rckyyf7849";            // WiFi şifreniz
const char* serverURL = "http://192.168.111.16:5000/api/door-status";
// ============================================

Servo myServo;
const int servoPin = 13;
const int buttonPin = 12;

bool servoState = false;
bool lastButtonState = HIGH;

void setup() {
  // Serial başlat - MUTLAKA 115200 olmalı
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n===================");
  Serial.println("ESP32 BASLATILIYOR");
  Serial.println("===================\n");
  
  // Servo kurulumu
  Serial.println("1. Servo baglaniyor...");
  myServo.attach(servoPin);
  myServo.write(0);
  Serial.println("   [OK] Servo hazir (0 derece)");
  
  // Buton kurulumu
  Serial.println("2. Buton pini ayarlaniyor...");
  pinMode(buttonPin, INPUT_PULLUP);  // Internal pull-up aktif
  Serial.println("   [OK] Buton hazir");
  
  // WiFi bağlantısı
  Serial.println("3. WiFi'ye baglaniyor...");
  Serial.print("   SSID: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("   [OK] WiFi baglandi!");
    Serial.print("   IP Adresi: ");
    Serial.println(WiFi.localIP());
    Serial.print("   Sinyal Gucu: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("   [HATA] WiFi baglanamiyor!");
    Serial.println("   WiFi adi ve sifresini kontrol edin!");
  }
  
  Serial.println("\n4. Sunucuya ilk veri gonderiliyor...");
  if (WiFi.status() == WL_CONNECTED) {
    sendDoorStatus("kapali");
  }
  
  Serial.println("\n===================");
  Serial.println("SISTEM HAZIR");
  Serial.println("===================\n");
  Serial.println("Butona basin ve izleyin...\n");
}

void loop() {
  bool buttonState = digitalRead(buttonPin);
  
  // Debug: Buton durumunu göster (sadece değiştiğinde)
  static bool lastPrintedState = HIGH;
  if (buttonState != lastPrintedState) {
    Serial.print("[DEBUG] Buton durumu: ");
    Serial.println(buttonState == LOW ? "BASILDI (LOW)" : "BIRAKILDI (HIGH)");
    lastPrintedState = buttonState;
  }
  
  // Butona basıldı mı? (HIGH'dan LOW'a geçiş)
  if (lastButtonState == HIGH && buttonState == LOW) {
    Serial.println("\n>>> BUTON ALGILANDI! <<<");
    
    servoState = !servoState;
    
    if (servoState) {
      Serial.println("-> Servo 90 dereceye donuyor...");
      myServo.write(90);
      Serial.println("-> Kapi ACILDI");
      sendDoorStatus("acik");
    } else {
      Serial.println("-> Servo 0 dereceye donuyor...");
      myServo.write(0);
      Serial.println("-> Kapi KAPANDI");
      sendDoorStatus("kapali");
    }
    
    delay(300); // Debounce
  }
  
  lastButtonState = buttonState;
}

void sendDoorStatus(String status) {
  Serial.println("\n--- HTTP POST Basliyor ---");
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[HATA] WiFi baglantisi yok!");
    Serial.println("WiFi yeniden baglaniyor...");
    WiFi.reconnect();
    delay(3000);
    return;
  }
  
  HTTPClient http;
  
  Serial.print("Sunucu: ");
  Serial.println(serverURL);
  
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000); // 5 saniye timeout
  
  // JSON verisi hazırla
  String jsonData = "{\"durum\":\"" + status + "\",\"zaman\":\"" + String(millis()) + "\"}";
  Serial.print("Gonderilen veri: ");
  Serial.println(jsonData);
  
  // POST isteği gönder
  int httpResponseCode = http.POST(jsonData);
  
  Serial.print("HTTP Yanit Kodu: ");
  Serial.println(httpResponseCode);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Sunucu Yaniti: ");
    Serial.println(response);
    Serial.println("[OK] Veri basariyla gonderildi!");
  } else {
    Serial.println("[HATA] HTTP istegi basarisiz!");
    Serial.print("Hata kodu: ");
    Serial.println(httpResponseCode);
    
    if (httpResponseCode == -1) {
      Serial.println("-> Sunucu acik degil veya IP adresi yanlis!");
    } else if (httpResponseCode == -11) {
      Serial.println("-> Baglanti zaman asimi!");
    }
  }
  
  http.end();
  Serial.println("--- HTTP POST Bitti ---\n");
}
