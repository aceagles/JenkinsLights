#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

ESP8266WebServer server(80);

struct LEDControl {
    int pin;
    String color;
    String state;
    
    LEDControl(int p, String c) : pin(p), color(c), state("OFF") {
        
    }
    void initialise(){
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }
    
    void setState(String newState) {
        state = newState;
        if(state == "ON")
          digitalWrite(pin, HIGH);
          
        if(state == "OFF")
          digitalWrite(pin, LOW);
        Serial.println(color);
        Serial.println(state);
    }
    
    void toggle() {
        if (state == "STROBE") {
            Serial.println(color);
            digitalWrite(pin, !digitalRead(pin));
        }
    }
};
LEDControl leds[] = {
      LEDControl(5, "RED"),
      LEDControl(4, "GREEN"),
      LEDControl(0, "AMBER"),
      LEDControl(2, "BLUE")
  };
const int numLeds = 4;
int findStringIndex(String& target) {
    String colors[] = {"RED", "GREEN", "AMBER", "BLUE"};
    for (int i = 0; i < 4; ++i) {
        if (target == colors[i]) {
            Serial.println(i);
            return i; // Return the index if a match is found
        }
    }
    return -1; // Return -1 if no match is found
}

void handleRoot() {
  String mode = server.arg("MODE");
  String color = server.arg("COLOUR");
  Serial.println(mode);
  Serial.println(color);

  leds[findStringIndex(color)].setState(mode);

  server.send(200, "text/plain", "OK");
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFiManager wifiManager;

  wifiManager.autoConnect("Jenkins Lights");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  //Create an array of LED instances
  for (int i = 0; i < numLeds; ++i) {
        leds[i].initialise();
    }
}

int loopcount = 0;

void loop() {
  server.handleClient();
  if(loopcount >= 20){
    loopcount = 0;
    for (int i = 0; i < numLeds; ++i) {
        leds[i].toggle();
    }
  }
  delay(50);
  loopcount++;
}