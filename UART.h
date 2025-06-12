#ifndef UART_H
#define UART_H

extern bool webTriggerAlgorithm2;
extern bool algorithm2Running;
extern char lastESP2Command;

void handleUARTSend()
{
    if (webTriggerAlgorithm2)
    {
        webTriggerAlgorithm2 = false;
        char cmdToSend = algorithm2Running ? 'P' : 'S';

        if (cmdToSend != lastESP2Command)
        {
            Serial.print("[UART] Sending '");
            Serial.print(cmdToSend);
            Serial.println("' to ESP2");
            Serial1.write(cmdToSend);
            lastESP2Command = cmdToSend;
        }
        else
        {
            Serial.println("[UART] Duplicate command ignored.");
        }
    }
}

#endif
