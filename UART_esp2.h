#ifndef UART_ESP2_H
#define UART_ESP2_H

#include <Arduino.h>
#include "pins2.h"

extern bool algorithmRunning;
extern int currentLED;
extern unsigned long delayTime;

void handleUARTReceive()
{
    if (Serial.available())
    {
        char command = Serial.read();
        Serial.print("[UART RX] Got: ");
        Serial.println(command);

        if (command == 'P')
        {
            Serial.println("[UART] Received 'P' → Start Algorithm 2");
            algorithmRunning = true;
            delayTime = 500;
        }
        else if (command == 'S')
        {
            Serial.println("[UART] Received 'S' → Pause Algorithm 2");
            algorithmRunning = false;
            for (int i = 0; i < NUM_LEDS; i++)
            {
                digitalWrite(LED1_PIN + i, LOW);
            }
            currentLED = -1;
        }
    }
}

#endif
