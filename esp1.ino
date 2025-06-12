#include <Arduino.h>
#include "pins.h"
#include "btn.h"
#include "wifi.h"
#include "UART.h"
#include "UART_RX.h"

unsigned long lastHoldTime = 0;
bool btnHold = false;
bool algorithmRunning = false;
bool webTriggerAlgorithm2 = false;
bool commandPAlreadySent = false;
bool algorithm2Running = false;
char lastESP2Command = '\0';
int currentLED = -1;
int ledPins[NUM_LEDS] = {LED1_PIN, LED2_PIN, LED3_PIN};

void setup()
{
    Serial.begin(SERIAL_SPEED, SERIAL_8O2);
    Serial1.begin(SERIAL_SPEED, SERIAL_8O2);

    pinMode(BUTTON_PIN, INPUT);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }

    initWiFi();
}

void loop()
{
    checkButtonHold();
    handleUARTSend();
    handleUARTReceive(); 

    static bool printedStart = false;
    static bool printedPause = false;

    if (algorithmRunning)
    {
        printedPause = false;

        if (!printedStart)
        {
            Serial.println("[INFO] Executing Algorithm 1...");
            printedStart = true;
        }

        if (currentLED >= 0)
            digitalWrite(ledPins[currentLED], LOW);

        currentLED = (currentLED + 1) % NUM_LEDS;
        digitalWrite(ledPins[currentLED], HIGH);

        Serial.print("LED ");
        Serial.println(currentLED + 1);

        delay(800);
    }
    else
    {
        printedStart = false;

        if (!printedPause)
        {
            Serial.println("[INFO] Algorithm 1 is paused.");
            printedPause = true;
        }

        for (int i = 0; i < NUM_LEDS; i++)
        {
            digitalWrite(ledPins[i], LOW);
        }

        currentLED = -1;
    }
}
