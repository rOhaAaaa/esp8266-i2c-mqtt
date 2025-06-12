#ifndef UART_RX_H
#define UART_RX_H

#include <Arduino.h>

extern bool algorithmRunning;

void handleUARTReceive()
{
    if (Serial1.available())
    {
        char command = Serial1.read();
        Serial.print("[CHECK] Got: ");
        Serial.println(command);

        if (command == 'P')
        {
            Serial.println("[UART] Received 'P' → Start Algorithm 1");
            algorithmRunning = true;
        }
        else if (command == 'S')
        {
            Serial.println("[UART] Received 'S' → Pause Algorithm 1");
            algorithmRunning = false;
        }
    }
}

#endif
