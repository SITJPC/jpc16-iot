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

// GPIO 27, 26, 25
int LED_RED = 27;
int LED_GREEN = 26;
int LED_BLUE = 25;

// GPIO 2
int LED_WIFI_STATUS = 2;

// ตั้งค่า Port ของ web server
WebServer server(80);

void setup() {
  // ตั้งค่าความเร็วระหว่างการส่งข้อมูลระหว่าง Microcontroller กับคอมของเรา
  Serial.begin(921600);

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
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_BLUE,OUTPUT);

  // ตั้งค่า path ให้กับ server ของเรา
  server.on("/", handleRoot);
  server.on("/on", HTTP_GET, ledOn);
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
  digitalWrite(LED_RED,HIGH);
  digitalWrite(LED_GREEN,HIGH);
  digitalWrite(LED_BLUE,HIGH);
  // ส่งค่ากลับ
  // 200 คือ ไม่มีข้อผิดพลาด
  server.send(200, "text/plain", "Hello from ESP32!");
}

void ledOn() {
  disablePrint = true;
  // Get the number of arguments in the request
  int args = server.args();

  // Loop through each argument and print its name and value
  for (int i = 0; i < args; i++) {
    if (server.argName(i) == "red") {
       digitalWrite(LED_RED, server.arg(i) == "1" ? HIGH : LOW);
    }

    if (server.argName(i) == "green") {
      digitalWrite(LED_GREEN, server.arg(i) == "1" ? HIGH : LOW);     
    }
    
    if (server.argName(i) == "blue") {
      digitalWrite(LED_BLUE, server.arg(i) == "1" ? HIGH : LOW);    
    }
  }

  server.send(200, "text/plain", "The light is on!");
}

void ledOff() {
  disablePrint = true;

  digitalWrite(LED_RED,LOW);
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(LED_BLUE,LOW);

  Serial.println("The lights are off!");
  server.send(200, "text/plain", "The light is off");
}

