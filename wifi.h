#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "indexHtml.h"
#include "pins.h"

extern bool algorithmRunning;
extern int currentLED;
extern bool webTriggerAlgorithm2;
extern bool algorithm2Running;
extern bool commandPAlreadySent;
extern char lastESP2Command;
extern int ledPins[NUM_LEDS];

AsyncWebServer server(80);

void initWiFi()
{
    WiFi.begin("Roha", "39865475");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        algorithmRunning = !algorithmRunning;
        commandPAlreadySent = false;

        Serial.println(algorithmRunning ? "[WEB] Algorithm 1 Started" : "[WEB] Algorithm 1 Paused");

        request->send(200, "text/plain", algorithmRunning ? "ON" : "OFF"); });

    server.on("/esp2", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("cmd")) {
            String cmd = request->getParam("cmd")->value();

            if ((cmd == "P" || cmd == "S") && cmd.charAt(0) != lastESP2Command)
            {
                webTriggerAlgorithm2 = true;
                algorithm2Running = (cmd == "P");
                lastESP2Command = cmd.charAt(0);

                Serial.print("[WEB] Command to ESP2 via UART: ");
                Serial.println(cmd);

                request->send(200, "text/plain", "Command sent: " + cmd);
            }
            else
            {
                request->send(200, "text/plain", "Command ignored");
            }
        } else {
            request->send(400, "text/plain", "Missing param");
        } });

    server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", String(currentLED)); });

    server.begin();
}

#endif
