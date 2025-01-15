/******************* WiFi Robot Remote Control Mode ********************/
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 
#include <ArduinoOTA.h>

#define ENA   14          // Enable/speed motors Right        GPIO14(D5)
#define ENB   12          // Enable/speed motors Left         GPIO12(D6)
#define IN_1  15          // L298N in1 motors Right          GPIO15(D8)
#define IN_2  13          // L298N in2 motors Right           GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Left            GPIO0(D3)

const int buzPin = D0;       // Set digital pin D0 as buzzer pin (use active buzzer)
const int wifiLedPin = D1;   // Set digital pin D1 for Wi-Fi connection LED (on when connected to Wi-Fi)

ESP8266WebServer server(80);      // Create a webserver object that listens for HTTP requests on port 80

String sta_ssid = "Airtel_mand_8436";      // Set Wi-Fi networks to connect to
String sta_password = "Air@06818";  // Set password for Wi-Fi networks
String command;           // String to store app command state
int SPEED = 1023;         // 330 - 1023, motor speed
int speed_Coeff = 3;      // Speed coefficient for turns

unsigned long previousMillis = 0;

void setup(){
  Serial.begin(115200);    // Set up Serial communication at 115200 bps
  Serial.println();
  Serial.println("*WiFi Robot Remote Control Mode*");
  Serial.println("--------------------------------------");

  // Initialize motor control pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);  
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT); 
  pinMode(buzPin, OUTPUT);  // Set buzzer pin as output
  pinMode(wifiLedPin, OUTPUT);  // Set Wi-Fi LED pin as output

  // Set NodeMCU Wi-Fi hostname based on chip MAC address
  String chip_id = String(ESP.getChipId(), HEX);
  int i = chip_id.length() - 4;
  chip_id = chip_id.substring(i);
  chip_id = "wificar-" + chip_id;
  String hostname(chip_id);
  
  Serial.println();
  Serial.println("Hostname: "+hostname);

  // Set NodeMCU to STA mode and try to connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  Serial.println("");
  Serial.print("Connecting to: ");
  Serial.println(sta_ssid);
  Serial.print("Password: ");
  Serial.println(sta_password);

  // Try to connect to Wi-Fi for up to 10 seconds
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis <= 10000) {
    delay(500);
    Serial.print(".");
    currentMillis = millis();
  }

  // If connection successful, set Wi-Fi LED on and print IP address
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("*WiFi-STA-Mode*");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(wifiLedPin, LOW);    // Wi-Fi LED on when connected to Wi-Fi as STA mode
    delay(3000);
  } else {
    // If Wi-Fi connection fails, switch to AP mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostname.c_str());
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("");
    Serial.println("WiFi failed to connect to " + sta_ssid);
    Serial.println("");
    Serial.println("*WiFi-AP-Mode*");
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    digitalWrite(wifiLedPin, HIGH);   // Wi-Fi LED off when in AP mode
    delay(3000);
  }

  // Start the web server
  server.on("/", HTTP_handleRoot);       
  server.onNotFound(HTTP_handleRoot);    
  server.begin();  // Actually start the server
  
  // Start OTA updates
  ArduinoOTA.begin();                       
}

void loop() {
    ArduinoOTA.handle();          // Listen for OTA update requests from clients
    server.handleClient();        // Listen for HTTP requests from clients
    
    // Check if a "State" argument is passed in the HTTP request
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
    else if (command == "W") TurnLightOn();
    else if (command == "w") TurnLightOff();
    else if (command == "0") SPEED = 330;
    else if (command == "1") SPEED = 400;
    else if (command == "2") SPEED = 470;
    else if (command == "3") SPEED = 540;
    else if (command == "4") SPEED = 610;
    else if (command == "5") SPEED = 680;
    else if (command == "6") SPEED = 750;
    else if (command == "7") SPEED = 820;
    else if (command == "8") SPEED = 890;
    else if (command == "9") SPEED = 960;
    else if (command == "q") SPEED = 1023;
}

// HTTP root handler
void HTTP_handleRoot(void) {
  server.send(200, "text/html", "");
  if (server.hasArg("State")) {
    Serial.println(server.arg("State"));
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}

// Motor control functions
void Forward() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, SPEED);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, SPEED);
}

void Backward() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, SPEED);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, SPEED);
}

void TurnRight() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, SPEED);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, SPEED);
}

void TurnLeft() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, SPEED);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, SPEED);
}

void ForwardRight() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, SPEED / speed_Coeff);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, SPEED);
}

void ForwardLeft() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, HIGH);
  analogWrite(ENA, SPEED);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, HIGH);
  analogWrite(ENB, SPEED / speed_Coeff);
}

void BackwardRight() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, SPEED / speed_Coeff);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, SPEED);
}

void BackwardLeft() {
  digitalWrite(IN_1, HIGH);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, SPEED);

  digitalWrite(IN_3, HIGH);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, SPEED / speed_Coeff);
}

void Stop() {
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);
  analogWrite(ENA, SPEED);

  digitalWrite(IN_3, LOW);
  digitalWrite(IN_4, LOW);
  analogWrite(ENB, SPEED);
}

// Function to beep the buzzer
void BeepHorn() {
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
  delay(80);
}
