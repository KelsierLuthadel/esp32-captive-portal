#include "display.h"
#include "portal.h"
#include "captiveWiFi.h"
#include "google.h"

Display display(true);
GooglePage googlePage;
CaptivePortal portal(&display, &googlePage, 33);

void setup() {
    display.init();
    display.clear(BLACK);

    display.setTextColor(WHITE);
    display.headingFont();

    display.setCursor(8, 0);
    display.output("Offensive ");
    display.setTextColor(YELLOW);
    display.outputln("Security");
    
    display.output("Creative ");
    display.setTextColor(WHITE);
    display.outputln("Destruction");

    display.setCursor(8, 64);
    display.setTextColor(SKYBLUE);
    display.outputln("The quieter you become,");
    display.outputln("the more you are able to hear");

    display.setTextColor(WHITE);
    display.setCursor(8, 128);
    display.outputln("Man is least himself when he");
    display.outputln("talks in his own person.");
    display.outputln("Give him a mask, and he will");
    display.outputln("tell you the truth.");

    portal.begin("UltraFast WiFi");
}

void loop() {
    portal.loop();
}