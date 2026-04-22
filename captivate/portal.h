#pragma once

#include <WiFi.h>
#include <DNSServer.h>
#include "PortalPage.h"

class CaptivePortal {
public:
    // Takes ownership of page — pass `new HackerWarningPage()` or any other PortalPage subclass
    CaptivePortal(PortalPage* page, int ledPin = 33);
    ~CaptivePortal();

    void begin(const char* ssid);
    void loop();

private:
    static const byte DNS_PORT = 53;
    static const IPAddress AP_IP;

    PortalPage* _page;
    PortalPage* _finalPage;
    int         _ledPin;
    DNSServer   _dnsServer;
    WiFiServer  _server;

    void   _blinkLED(int times, int onMs = 100, int offMs = 100);
    void   _handleClient(WiFiClient& client);
    String _extractValue(const String& body, const String& key);
    String _urlDecode(const String& input);
};