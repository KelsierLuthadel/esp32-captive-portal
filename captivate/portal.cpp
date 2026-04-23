#include "portal.h"
#include "warningpage.h"
#include "display.h"
#include <WiFi.h>
#include "esp_wifi.h"
#include <map>

const IPAddress CaptivePortal::AP_IP(8, 8, 4, 4);

// ------------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------------
CaptivePortal::CaptivePortal(Display *display, PortalPage* page, int ledPin)
    : m_display(display), m_page(page), m_ledPin(ledPin), m_server(80), m_connections(0), m_logins(0) {
      m_finalPage =  new HackerWarningPage();
    }

CaptivePortal::~CaptivePortal() {
    delete m_page;
}

// ------------------------------------------------------------
// Public
// ------------------------------------------------------------
void CaptivePortal::begin(const char* ssid) {
    pinMode(m_ledPin, OUTPUT);
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));

    blinkLED(5);

    m_dnsServer.start(DNS_PORT, "*", AP_IP);
    m_server.begin();
}

void CaptivePortal::loop() {
    m_dnsServer.processNextRequest();

    WiFiClient client = m_server.available();
    if (client) {
        handleClient(client);
    }
}

// ------------------------------------------------------------
// Private
// ------------------------------------------------------------
void CaptivePortal::blinkLED(int times, int onMs, int offMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(m_ledPin, HIGH);
        delay(onMs);
        digitalWrite(m_ledPin, LOW);
        delay(offMs);
    }
}

void CaptivePortal::handleClient(WiFiClient& client) {
    String currentLine   = "";
    String requestBody   = "";
    String requestLine   = "";

    bool isPost          = false;
    bool headersDone     = false;
    bool requestCaptured = false;

    int contentLength = 0;

    while (client.connected()) {
        if (!client.available()) {
            continue;
        }

        char c = client.read();

        if (!headersDone) {

            // build line by line
            if (c == '\n') {

                // capture first HTTP request line ONLY once
                if (!requestCaptured) {
                    requestLine = currentLine;
                    requestCaptured = true;
                }

                if (currentLine.startsWith("POST")) {
                    isPost = true;
                }

                if (currentLine.startsWith("Content-Length:")) {
                    contentLength = currentLine.substring(16).toInt();
                }

                // end of headers
                if (currentLine.length() == 0) {
                    headersDone = true;

                    // read POST body if needed
                    if (contentLength > 0) {
                        for (int i = 0; i < contentLength; i++) {
                            while (!client.available());
                            requestBody += (char)client.read();
                        }

                        this->m_logins++;
                        this->stats();
                        String email    = urlDecode(extractValue(requestBody, "email"));
                       
                    }
                }

                currentLine = "";
            }
            else if (c != '\r') {
                currentLine += c;
            }
        }

        if (headersDone) {

            if (!isPost) {
                bool ignoreRequest =
                    (requestLine.indexOf("generate_204") != -1) ||
                    (requestLine.indexOf("favicon.ico") != -1) ||
                    (requestLine.indexOf("hotspot-detect") != -1) ||
                    (requestLine.indexOf("apple-touch-icon") != -1);

                if (!ignoreRequest) {
                    this->m_connections++;
                    this->stats();
                }

                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();
                client.print(m_page->html());
            }
            else {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();
                client.print(m_finalPage->html());
            }

            break;
        }
    }

    client.stop();
}

void CaptivePortal::stats() {
    String conn = String(this->m_connections);
    String login = String(this->m_logins);
    String score = conn + "x" + login;

    this->m_display->clear(BLACK);

    this->m_display->setTextColor(WHITE);
    this->m_display->headingFont();

    this->m_display->setCursor(8, 0);
    this->m_display->output("Offensive ");
    this->m_display->setTextColor(YELLOW);
    this->m_display->outputln("Security");

    this->m_display->setTextColor(WHITE);
    this->m_display->output("Creative ");
    this->m_display->outputln("Destruction");
   
    this->m_display->headingFont();
    this->m_display->setTextColor(SKYBLUE);
    this->m_display->resetCursor(8,96);

    this->m_display->outputln("The quieter you become,");
    this->m_display->outputln("the more you are able to hear");

    this->m_display->setTextColor(WHITE);
    this->m_display->cursorBottom(

    );
    this->m_display->output_right(score);
    
}

String CaptivePortal::extractValue(const String& body, const String& key) {
    int start = body.indexOf(key + "=");
    if (start == -1) return "";
    start += key.length() + 1;
    int end = body.indexOf("&", start);
    if (end == -1) end = body.length();
    return body.substring(start, end);
}

String CaptivePortal::urlDecode(const String& input) {
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

void CaptivePortal::getConnectedMacs(std::vector<String>& macs) {
    wifi_sta_list_t sta_list;

    esp_wifi_ap_get_sta_list(&sta_list);

    for (int i = 0; i < sta_list.num; i++) {
        wifi_sta_info_t station = sta_list.sta[i];

        char macStr[18];
        sprintf(macStr,
                "%02X:%02X:%02X:%02X:%02X:%02X",
                station.mac[0], station.mac[1], station.mac[2],
                station.mac[3], station.mac[4], station.mac[5]);

        macs.push_back(String(macStr));
    }
}

String CaptivePortal::processClient() {
    std::vector<String> macs;
    getConnectedMacs(macs);

    for (auto &mac : macs) {

        // first time seen device
        if (!m_seenMacs.count(mac)) {
            m_seenMacs[mac] = true;
            Serial.println("New device MAC: " + mac);
        }
    }
}