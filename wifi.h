#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

extern bool algorithmRunning;
extern bool webTrigger;

AsyncWebServer server(80);
bool wifiConnected = false;

#define SSID "307_2.4G"
#define PASSWORD "29012006"

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void initWiFi() {
    if (wifiConnected) return;

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    Serial.print("Connecting to WiFi...");
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
        delay(1000);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: http://");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi. Check SSID/PASSWORD.");
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
        algorithmRunning = !algorithmRunning;
        webTrigger = true;
        Serial.println(algorithmRunning ? "Algorithm Resumed (via Web)" : "Algorithm Paused (via Web)");
        request->send(200, "text/plain", "OK");
    });

    server.onNotFound(notFound);
    server.begin();
}

#endif
