#include <HTTPClient.h>
#include <WiFi.h>
#include <WebServer.h>

// ชื่อ Wifi และ Password ที่จะเชื่อมต่อ
const char* WIFI_STA_NAME = "test_wifi";
const char* WIFI_STA_PASS = "test_password";

// IP Address
char IP_ADDRESS[15];
// Note: Your IP Address: 8.8.8.8

// เอาไว้เพื่อปิดไม่ให้แสดงค่า IP เฉย ๆ 
bool disablePrint = false;

// GPIO 27
int LED_PIN = 27;
// GPIO 2
int LED_WIFI_STATUS = 2;

// ตั้งค่า Port ของ web server
WebServer server(80);

void setup() {
  // ตั้งค่าความเร็วระหว่างการส่งข้อมูลระหว่าง Microcontroller กับคอมของเรา
  Serial.begin(1000000);

  // ตั้งค่า pin ของ LED แสดงสถานะ Wifi เป็นการส่งข้อมูล
  pinMode(LED_WIFI_STATUS, OUTPUT);

  // เชื่อมต่อ Wfi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

  // Loop ไปจนกว่าจะสามารถเชื่อมต่อ Wifi ได้
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay(500);
    digitalWrite(LED_WIFI_STATUS, LOW);
  }

  // เก็บเลข IP ลงตัวแปร
  for (int i = 0; i < sizeof(IP_ADDRESS); ++i) {
    IP_ADDRESS[i] = WiFi.localIP().toString().charAt(i);
  }

  digitalWrite(LED_WIFI_STATUS, HIGH);
  pinMode(LED_PIN,OUTPUT);

  // ตั้งค่า path ให้กับ server ของเรา
  server.on("/", handleRoot);
  server.on("/on", ledOn);
  server.on("/off", ledOff);
  server.begin();
}

void loop() {
  if (!disablePrint) {
    delay(500);
    printWifiInfo();
  }

  // เปิดให้ esp32 สามารถรับ request ได้
  server.handleClient();
}

void printWifiInfo() {
  // print IP_ADDRESS
  Serial.print("Your IP Address is: ");
  for (int i = 0; i < sizeof(IP_ADDRESS); ++i) {
    Serial.print(IP_ADDRESS[i]);
  }
  Serial.println();
}

void handleRoot() {
  disablePrint = true;
  // ส่งค่ากลับ
  // 200 คือ ไม่มีข้อผิดพลาด
  server.send(200, "text/plain", "Hello from ESP32!");
}

void ledOn() {
  disablePrint = true;
  digitalWrite(LED_PIN,HIGH);
  Serial.println("The light is on!");
  server.send(200, "text/plain", "The light is on!");
}

void ledOff() {
  disablePrint = true;
  digitalWrite(LED_PIN,LOW);
  Serial.println("The light is off!");
  server.send(200, "text/plain", "The light is off");
}
