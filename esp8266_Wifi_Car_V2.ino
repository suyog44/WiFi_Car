/******************* WiFi Robot Remote Control Mode ********************/
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ArduinoOTA.h>

// Motor Control Pins (L298N)
#define ENA   14          // Enable/speed motors Right (D5)
#define ENB   12          // Enable/speed motors Left  (D6)
#define IN_1  15          // Motor Right IN1 (D8)
#define IN_2  13          // Motor Right IN2 (D7)
#define IN_3  2           // Motor Left  IN3 (D4)
#define IN_4  4           // Motor Left  IN4 (D2) - Changed from GPIO0

// Peripheral Pins
const int buzPin = D0;       // Active buzzer
const int wifiLedPin = D1;   // Wi-Fi status LED
const int headlightPin = D3; // Headlight (newly added)

ESP8266WebServer server(80);

// WiFi Credentials
String sta_ssid = "Airtel_mand_8436";
String sta_password = "Air@06818";

// Control Variables
String command;             
int SPEED = 1023;          // Default speed (PWM range)
int speed_Coeff = 3;       // Turning speed coefficient

void setup(){
  Serial.begin(115200);
  Serial.println("\n*WiFi Robot Remote Control Mode*");

  // Pin Initialization
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);  
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(buzPin, OUTPUT);
  pinMode(wifiLedPin, OUTPUT);
  pinMode(headlightPin, OUTPUT);  // Initialize headlight
  
  // Motor Safety
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);

  // WiFi Setup
  String hostname = "wificar-" + String(ESP.getChipId(), HEX).substring(2);
  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  
  Serial.println("\nConnecting to: " + sta_ssid);
  unsigned long timeout = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - timeout < 10000) {
    Serial.print(".");
    delay(500);
  }

  // Connection Status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nSTA Mode - IP: " + WiFi.localIP().toString());
    digitalWrite(wifiLedPin, LOW);
  } else {
    WiFi.softAP(hostname.c_str());
    Serial.println("\nAP Mode - IP: " + WiFi.softAPIP().toString());
    digitalWrite(wifiLedPin, HIGH);
  }

  // Server Handlers
  server.on("/", HTTP_handleRoot);
  server.onNotFound(HTTP_handleRoot);
  server.begin();

  // OTA Updates
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  
  command = server.arg("State");
  if (command == "F") Forward();
  else if (command == "B") Backward();
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "G") ForwardLeft();
  else if (command == "H") BackwardLeft();
  else if (command == "I") ForwardRight();
  else if (command == "J") BackwardRight();
  else if (command == "S") Stop();
  else if (command == "V") BeepHorn();
  else if (command == "W") digitalWrite(headlightPin, HIGH);
  else if (command == "w") digitalWrite(headlightPin, LOW);
  else if (command >= "0" && command <= "9") SPEED = map(command.toInt(), 0, 9, 330, 960);
  else if (command == "q") SPEED = 1023;
}

// Motor Control Functions
void Forward() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
}

void Backward() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
}

void TurnRight() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
}

void TurnLeft() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
}

void Stop() {  // Full stop with power cutoff
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void BeepHorn() {
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
}

// Web Server Handler
void HTTP_handleRoot() {
  server.send(200, "text/html", 
    "<html>"
    "<body style='text-align:center'>"
    "<h1>Robot Control</h1>"
    "<p>Speed: " + String(SPEED) + "</p>"
    "</body>"
    "</html>"
  );
}
