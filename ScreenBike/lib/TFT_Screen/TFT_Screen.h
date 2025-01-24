#ifndef TFT_SCREEN_H
#define TFT_SCREEN_H


#define LOAD_GFXX

// #include <Adafruit_GFX.h>
// #include <Adafruit_ST7789.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Thermocouple.h>
#include <MAX6675_Thermocouple.h>
#include <ClosedCube_SHT31D.h>

/* Font */
#include "Andromeda_Bold_Tiny_10.h"
#include "Andromeda_Bold_Tiny_20.h"
#include "Andromeda_Bold_Tiny_25.h"
#include "Andromeda_Bold_Tiny_15.h"

/* Icon */
#include "TSR_176x160.h"
#include "GArrow_Up_28x15.h"
#include "Accu_34x26.h"
#include "Arrow_Down 28x15.h"
#include "Timer 28x33.h"
#include "Temperature_Yellow_26x20.h"
#include "Temperature_Yellow_14x24.h"
#include "Temperature_Red_38x29.h"
#include "Temperature_Blue_28x29.h"
#include "Huminity_16x19.h"
#include "Engine_RED_36x23.h"
#include "Engine_Blue_36x23.h"
#include "TFT_Image.h"



#define TFT_CS 15
#define TFT_RST 25  
#define TFT_DC 20
#define TFT_MOSI 13 // Data out
#define TFT_SCLK 14 // Clock out
#define TFT_BL 12   // BLACK LIGHT

#define VMax 1
#define VNow 2
#define VMin 3
#define SHT30_TEMP 4
#define SHT30_HUMI 5
#define THERMAL_1 6
#define THERMAL_2 7


class TFT_Screen
{
private:
    // Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
    /* TFT Display */
    TFT_eSPI tft = TFT_eSPI();
    
    ClosedCube_SHT31D &sht3xd;
    SHT31D &sht3xd_data;

    Thermocouple *thermocouple;
    Thermocouple *thermocouple1;

public:

    TFT_Screen(ClosedCube_SHT31D &pSHT3XD, SHT31D pResult);
    void begin();
    void drawLogo();
    void setBlackLight(bool _stateScreen);
    bool compareDataOnScreen(float a, float b);
    void showCounter(uint32_t hours, uint32_t minutes, uint32_t seconds,  int16_t x, int16_t y, uint16_t colour, uint16_t bg);
    void showData(uint8_t screen, uint16_t _tx, uint16_t _ty, float data, uint16_t x, uint16_t y, uint16_t fillX, uint16_t fillY, int32_t w, int32_t h, uint8_t decimal, uint16_t colour, uint32_t bg);

};
#endif