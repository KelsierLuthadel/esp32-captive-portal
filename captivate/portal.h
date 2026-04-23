#pragma once

#include <WiFi.h>
#include <DNSServer.h>
#include "PortalPage.h"
#include "display.h"
#include <map>

class CaptivePortal {
public:
    // Takes ownership of page — pass `new HackerWarningPage()` or any other PortalPage subclass
    CaptivePortal(Display* display, PortalPage* page, int ledPin = 33);
    ~CaptivePortal();

    void begin(const char* ssid);
    void loop();

private:
    static const byte DNS_PORT = 53;
    static const IPAddress AP_IP;

    Display*    m_display;
    PortalPage* m_page;
    PortalPage* m_finalPage;
    int         m_ledPin;
    DNSServer   m_dnsServer;
    WiFiServer  m_server;
    unsigned int m_connections;
    unsigned int m_logins;
    std::map<String, bool> m_seenMacs;


    void   blinkLED(int times, int onMs = 100, int offMs = 100);
    void   handleClient(WiFiClient& client);
    String extractValue(const String& body, const String& key);
    String urlDecode(const String& input);
    void   stats();
    void getConnectedMacs(std::vector<String>& macs);
    String processClient();
};