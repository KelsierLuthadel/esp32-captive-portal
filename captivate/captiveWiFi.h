#pragma once
#include "portalpage.h"

class CaptiveWiFi : public PortalPage {
public:
    String html() const override;
};