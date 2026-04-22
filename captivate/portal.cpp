#include "portal.h"
#include "warningpage.h"

const IPAddress CaptivePortal::AP_IP(8, 8, 4, 4);

CaptivePortal::CaptivePortal(PortalPage* page, int ledPin)
    : _page(page), _ledPin(ledPin), _server(80) {
      _finalPage =  new HackerWarningPage();
    }

CaptivePortal::~CaptivePortal() {
    delete _page;
}

// ------------------------------------------------------------
// Public
// ------------------------------------------------------------
void CaptivePortal::begin(const char* ssid) {
    pinMode(_ledPin, OUTPUT);
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));

    _blinkLED(5);

    _dnsServer.start(DNS_PORT, "*", AP_IP);
    _server.begin();
}

void CaptivePortal::loop() {
    _dnsServer.processNextRequest();

    WiFiClient client = _server.available();
    if (client) {
        _handleClient(client);
    }
}

// ------------------------------------------------------------
// Private
// ------------------------------------------------------------
void CaptivePortal::_blinkLED(int times, int onMs, int offMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(_ledPin, HIGH);
        delay(onMs);
        digitalWrite(_ledPin, LOW);
        delay(offMs);
    }
}

void CaptivePortal::_handleClient(WiFiClient& client) {
    String currentLine   = "";
    String requestBody   = "";
    bool   isPost        = false;
    bool   headersDone   = false;
    int    contentLength = 0;

    while (client.connected()) {
        if (!client.available()) continue;

        char c = client.read();

        if (!headersDone) {
            if (c == '\n') {
                if (currentLine.startsWith("POST")){
                    isPost = true;
                }   
                if (currentLine.startsWith("Content-Length:")){
                    contentLength = currentLine.substring(16).toInt();
                } 

                if (currentLine.length() == 0) {
                    headersDone = true;

                    if (contentLength > 0) {
                        for (int i = 0; i < contentLength; i++) {
                            while (!client.available());
                            requestBody += (char)client.read();
                        }
                        String email = _urlDecode(_extractValue(requestBody, "email"));
                    }
                }
                currentLine = "";
            } else if (c != '\r') {
                currentLine += c;
            }
        }

        if (headersDone) {
            if (!isPost) {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();
                client.print(_page->html());
            } else {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();
                client.print(_finalPage->html());

            }
            break;
        }
    }

    client.stop();
}

String CaptivePortal::_extractValue(const String& body, const String& key) {
    int start = body.indexOf(key + "=");
    if (start == -1) return "";
    start += key.length() + 1;
    int end = body.indexOf("&", start);
    if (end == -1) end = body.length();
    return body.substring(start, end);
}

String CaptivePortal::_urlDecode(const String& input) {
    String output = "";
    for (int i = 0; i < input.length(); i++) {
        if (input[i] == '+') {
            output += ' ';
        } else if (input[i] == '%' && i + 2 < input.length()) {
            char hex[3] = { input[i + 1], input[i + 2], '\0' };
            output += (char)strtol(hex, nullptr, 16);
            i += 2;
        } else {
            output += input[i];
        }
    }
    return output;
}