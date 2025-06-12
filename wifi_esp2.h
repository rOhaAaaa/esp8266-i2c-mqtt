#ifndef WIFI_ESP2_H
#define WIFI_ESP2_H

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "indexHtml_esp2.h"

extern bool algorithmRunning;
extern bool algorithm1Active;
extern int currentLED;
extern unsigned long delayTime;

AsyncWebServer server(80);

void initWiFi()
{
    WiFi.begin("Roha", "39865475");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        algorithmRunning = !algorithmRunning;
        Serial.println(algorithmRunning ? "[WEB] Algorithm 2 Started" : "[WEB] Algorithm 2 Paused");
        request->send(200, "text/plain", algorithmRunning ? "Algorithm Started" : "Algorithm Stopped"); });

    server.on("/toggle_alg1", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        algorithm1Active = !algorithm1Active;
        char cmd = algorithm1Active ? 'P' : 'S';
        Serial.write(cmd);
        Serial.print("[WEB] Sent to ESP1: ");
        Serial.println(cmd);
        request->send(200, "text/plain", algorithm1Active ? "Started Algorithm 1" : "Stopped Algorithm 1"); });

    server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", String(currentLED)); });

    server.begin();
}

#endif
