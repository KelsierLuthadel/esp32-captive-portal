#pragma once
#include <Arduino.h>

class PortalPage {
public:
    virtual String html() const = 0;
    virtual ~PortalPage() = default;
};