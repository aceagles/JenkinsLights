#include <Arduino.h>
#include <ESP8266WiFi.h>         
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         

ESP8266WebServer server(80);

struct LEDControl {
    int pin;
    String color;
    String state;
    
    LEDControl(int p, String c) : pin(p), color(c), state("OFF") {
        
    }
    void initialise(){
      digitalWrite(pin, LOW);
      pinMode(pin, OUTPUT);
      
    }
    
    void setState(String newState) {
        if (newState == "ON" || newState == "OFF" || newState == "STROBE"){
          state = newState;
        }
        if(state == "ON")
          digitalWrite(pin, HIGH);
          
        if(state == "OFF")
          digitalWrite(pin, LOW);
        Serial.println(color);
        Serial.println(state);
    }
    
    void toggle() {
      // Toggles the pin if in STROBE mode
      if (state == "STROBE") {
          digitalWrite(pin, !digitalRead(pin));
      }
    }
};

// Array of leds for control
LEDControl leds[] = {
      LEDControl(5, "RED"),
      LEDControl(4, "GREEN"),
      LEDControl(14, "AMBER"),
      LEDControl(15, "BLUE")
  };

const int numLeds = 4;
int findStringIndex(String& target) {
    // This should really use the leds array but since this is a one off won't bother
    String colors[] = {"RED", "GREEN", "AMBER", "BLUE"};
    for (int i = 0; i < 4; ++i) {
        if (target == colors[i]) {
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
  // Select the correct LED using the find index function then setmode
  if(findStringIndex(color) >= 0)
    leds[findStringIndex(color)].setState(mode);

  server.send(200, "text/plain", "OK");
}



void setup() {
  for (int i = 0; i < numLeds; ++i) {
        leds[i].initialise();
    }
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFiManager wifiManager;

  wifiManager.autoConnect("Jenkins Lights");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  //Create an array of LED instances
  
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