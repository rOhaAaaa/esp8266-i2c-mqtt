#ifndef BTN_H
#define BTN_H

#include <Arduino.h>
#include "pins.h"

extern bool algorithmRunning;
extern unsigned long lastHoldTime;
extern bool btnHold;
extern int currentLED;
extern int ledPins[NUM_LEDS];

void checkButtonHold()
{
    bool buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == HIGH)
    {
        if (!btnHold)
        {
            lastHoldTime = millis();
            btnHold = true;
            Serial.println("[BTN] Button pressed...");
        }

        if (millis() - lastHoldTime >= HOLD_TIME && btnHold)
        {
            algorithmRunning = !algorithmRunning;

            if (algorithmRunning)
            {
                Serial.println("[BTN] Algorithm 1 Started via Button");
            }
            else
            {
                Serial.println("[BTN] Algorithm 1 Paused via Button");
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    digitalWrite(ledPins[i], LOW);
                }
                currentLED = -1;
            }

            btnHold = false;
        }
    }
    else
    {
        btnHold = false;
    }
}

#endif
