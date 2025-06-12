#include <Arduino.h>
#include "pins2.h"
#include "wifi_esp2.h"
#include "btn2.h"
#include "UART_esp2.h"

bool algorithmRunning = false;
bool algorithm1Active = false;
bool btnHold = false;
unsigned long lastHoldTime = 0;
int currentLED = -1;
unsigned long delayTime = 500;
int ledPins[NUM_LEDS] = {LED3_PIN, LED2_PIN, LED1_PIN};

void setup()
{
    Serial.begin(SERIAL_SPEED, SERIAL_8O2);  
    pinMode(BUTTON_PIN, INPUT_PULLUP);

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
    handleUARTReceive();

    static bool printedStart = false;
    static bool printedPause = false;

    if (algorithmRunning)
    {
        printedPause = false;

        if (!printedStart)
        {
            Serial.println("[INFO] Executing Algorithm 2...");
            printedStart = true;
        }

        if (currentLED >= 0)
            digitalWrite(ledPins[currentLED], LOW);

        currentLED = (currentLED + 1) % NUM_LEDS;
        digitalWrite(ledPins[currentLED], HIGH);

        Serial.print("LED ");
        Serial.println(currentLED + 1);

        delay(delayTime);
        delayTime += 200;
    }
    else
    {
        printedStart = false;

        if (!printedPause)
        {
            Serial.println("[INFO] Algorithm 2 is paused.");
            printedPause = true;
        }

        for (int i = 0; i < NUM_LEDS; i++)
        {
            digitalWrite(ledPins[i], LOW);
        }

        currentLED = -1;
    }
}
