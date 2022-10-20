/*
 *  chandy-basic-webserver
 *
 *  Serve a basic webpage for controlling lights and motors on the chandlier
 *
 */
#include "WiFi.h"

void setup()
{
    Serial.begin(115200);

#if 0
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
#endif
    delay(100);

    Serial.println("Setup done");
}

void loop()
{
    Serial.println("top of loop");
    Serial.println("");

    // Wait a bit before looping again
    delay(5000);
}
