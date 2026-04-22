#include "portal.h"
#include "captiveWiFi.h"

CaptivePortal portal(new CaptiveWiFi(), 33);

void setup() {
    portal.begin("UltraFast WiFi");
}

void loop() {
    portal.loop();
}
