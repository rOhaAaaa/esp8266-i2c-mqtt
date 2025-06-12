#ifndef BTN_H
#define BTN_H

#include <Arduino.h>
#include "pins2.h"

extern bool algorithmRunning;
extern unsigned long lastHoldTime;
extern bool btnHold;
extern int currentLED;
extern unsigned long delayTime;

void checkButtonHold()
{
    bool buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW)
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
                delayTime = 500;
                Serial.println("[BTN] Algorithm 2 Started via Button");
            }
            else
            {
                Serial.println("[BTN] Algorithm 2 Paused via Button");
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    digitalWrite(LED1_PIN + i, LOW);
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
