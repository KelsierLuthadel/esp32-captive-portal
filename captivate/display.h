#ifndef _KELSIER_DISPLAY_H_
#define _KELSIER_DISPLAY_H_

#include <Arduino.h>
#include "gfxfont.h"

// 16-bit RGB565 color definitions
#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xD69A
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFDA0
#define GREENYELLOW 0xB7E0
#define PINK        0xFE19
#define BROWN       0x9A60
#define GOLD        0xFEA0
#define SILVER      0xC618
#define SKYBLUE     0x867D
#define VIOLET      0x915C

class Display {

public:
  explicit Display(bool serial = false);

  void init();
  void rotate(uint8_t r);

  void clear();
  void clear(uint16_t color);

  // Fonts
  void titleFont();
  void headingFont();
  void bodyFont();

  // State / configuration
  void setScreenColor(uint16_t color);
  void setCursor(int16_t x, int16_t y);
  void resetCursor(int16_t x, int16_t y);
  void cursorBottom();


  void setFont(const GFXfont *f = nullptr);
  void setTextSize(uint8_t size);

  void setTextColor(uint16_t fg_color);
  void setTextColor(uint16_t fg_color, uint16_t bg_color, bool bgfill = false);

  // Output (preferred: const char*)
  void output(int32_t x, int32_t y, const char* value);
  void output(const char* value);
  void output(const String& value);

  void output_right(const char* value);
  void output_right(const String& value);

  void outputln(const char* value);
  void outputln(const String& value);

  // Metrics
  uint16_t textWidth(const char *string) const;
  uint16_t fontHeight() const;

private:
  bool m_serialOutput;
  uint16_t m_color;

  int16_t m_xPos;
  int16_t m_base_xPos;
  int16_t m_yPos;
};

#endif