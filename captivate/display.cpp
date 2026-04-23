#include <TFT_eSPI.h>
#include <SPI.h>
#include "display.h"
#include "gfxfont.h"

TFT_eSPI tft = TFT_eSPI();

Display::Display(bool serial)
: m_serialOutput(serial),
  m_xPos(0),
  m_yPos(0),
  m_base_xPos(0),
  m_color(TFT_WHITE)
{
}

void Display::init()
{
  tft.begin();
  rotate(1);

  clear(m_color);

  bodyFont();

  setTextColor(TFT_BLACK);
  setCursor(0, 0);
}

void Display::titleFont() 
{
  setFont(&FreeMono24pt7b);
}

void Display::headingFont() 
{
  setFont(&FreeMono12pt7b);
}

void Display::bodyFont()
{
  setFont(&FreeMono9pt7b);
}

void Display::setCursor(int16_t x, int16_t y)
{
  m_xPos = x;
  m_base_xPos = x;
  m_yPos = y;
  tft.setCursor(x, y); 
}

void Display::resetCursor(int16_t x, int16_t y)
{
  m_xPos = x;
  m_yPos = y;
  tft.setCursor(x, y);
}

void Display::cursorBottom()
{
  resetCursor(0, tft.height() - fontHeight() - 8);
}

void Display::rotate(uint8_t r)
{
  tft.setRotation(r);
}

void Display::clear()
{
  tft.fillScreen(m_color);
}

void Display::clear(uint16_t color)
{
  m_color = color;
  tft.fillScreen(color);
}

void Display::setScreenColor(uint16_t color)
{
  m_color = color;
}

void Display::setFont(const GFXfont *font)
{
  tft.setFreeFont(font); 
}

void Display::setTextSize(uint8_t size)
{
  tft.setTextSize(size);
}

void Display::setTextColor(uint16_t fg_color)
{
  tft.setTextColor(fg_color);
}

void Display::setTextColor(uint16_t fg_color, uint16_t bg_color, bool bgfill)
{
  tft.setTextColor(fg_color, bg_color, bgfill);
}

void Display::output(int32_t x, int32_t y, const char* value)
{
  tft.drawString(value, x, y);
}

void Display::output(const char* value)
{
  tft.drawString(value, m_xPos, m_yPos);
  m_xPos += textWidth(value);
}

void Display::output(const String& value)
{
  output(value.c_str());
}

void Display::output_right(const char* value)
{
  int16_t screen_width = tft.width();
  int16_t str_width = textWidth(value);
  int16_t x = screen_width - str_width;

  if (x < 0) x = 0; // basic bounds safety

  tft.drawString(value, x, m_yPos);
}

void Display::output_right(const String& value)
{
  output_right(value.c_str());
}

void Display::outputln(const char* value)
{
  tft.drawString(value, m_xPos, m_yPos);
  m_xPos = m_base_xPos;
  m_yPos += fontHeight();
}

void Display::outputln(const String& value)
{
  outputln(value.c_str());
}

uint16_t Display::textWidth(const char *string) const
{
  return tft.textWidth(string);
}

uint16_t Display::fontHeight() const
{
  return tft.fontHeight();
}