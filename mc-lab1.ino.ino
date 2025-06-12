#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiClient.h>
#include "indexHtml.h"
#include "wifi.h"

#define BUTTON_PIN 14  
#define LED1_PIN 12   
#define LED2_PIN 2   
#define LED3_PIN 0    

#define HOLD_TIME 2000  

#define NUM_LEDS 3 

unsigned long lastHoldTime = 0;  
bool btnHold = false;            
bool algorithmRunning = true;   
bool webTrigger = false; 

int ledPins[] = {LED1_PIN, LED2_PIN, LED3_PIN};
int currentLED = 0;  

unsigned long currentDelay = 0;

void buttonHold() {
  bool buttonState = digitalRead(BUTTON_PIN);  

  if (buttonState == HIGH) {
    if (!btnHold) {  
      lastHoldTime = millis();  
      btnHold = true;
      Serial.println("Button PRESSED");
    }

    if (millis() - lastHoldTime >= HOLD_TIME) {
      algorithmRunning = !algorithmRunning;  
      if (algorithmRunning) {
        Serial.println("Algorithm Resumed");
      } else {
        Serial.println("Algorithm Paused");
      }
      btnHold = false;  
    }
  } else {
    btnHold = false;  
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);  
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);    
    digitalWrite(ledPins[i], LOW);  
  }

  initWiFi();  
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
    algorithmRunning = !algorithmRunning;  
    Serial.println(algorithmRunning ? "Algorithm Resumed (via Web)" : "Algorithm Paused (via Web)");
    request->send(200, "text/plain", algorithmRunning ? "ON" : "OFF");
  });

  server.begin();
}

void loop() {
  buttonHold(); 

   if (algorithmRunning) {
    digitalWrite(ledPins[currentLED], LOW); 
    currentLED = (currentLED + 1) % NUM_LEDS;  
    digitalWrite(ledPins[currentLED], HIGH); 
    Serial.print("LED ");
    Serial.println(currentLED + 1); 
    delay(1000);  
  }

  delay(200);  
}
