#include "TFT_Screen.h"

TFT_Screen::TFT_Screen(ClosedCube_SHT31D &pSHT3XD, SHT31D pSHT3XD_Data) : sht3xd(pSHT3XD),
                                                                          sht3xd_data(pSHT3XD_Data)
{
}

void TFT_Screen::begin()
{
    pinMode(TFT_BL, OUTPUT);
    setBlackLight(false);
    tft.init();
    // tft.init(172, 320);
    tft.fillScreen(TFT_BLACK);
    setBlackLight(true);

    tft.setRotation(3);
    /* Introduce the Logo */
    // tft.drawRGBBitmap(65, -30, LOGO, 172, 243);
    // tft.setSwapBytes(true);
    // tft.drawBitmap(0, 0, LOGO, 5, 5, TFT_WHITE);
    // tft.pushImage(80, -30, 172, 243, LOGO);
    // delay(3000);
    // for (size_t y = 0; y < 243; y++)
    // {
    //     static uint16_t x = 0;
    //     tft.fillRect(x, y, 243, 10, TFT_BLACK);
    //     x += 1;
    //     y += 1;
    //     delay(20);
    // }

    tft.fillScreen(TFT_BLACK);
    // tft.setFreeFont(&Andromeda_Bold24pt7b);
}

void TFT_Screen::setBlackLight(bool _stateScreen)
{
    digitalWrite(TFT_BL, _stateScreen);
}

bool TFT_Screen::compareDataOnScreen(float a, float b)
{
    int16_t int_a = (int16_t)(a * 100);
    int16_t int_b = (int16_t)(b * 100);

    return int_a == int_b;
}

void TFT_Screen::showData(uint8_t screen, uint16_t _tx, uint16_t _ty, float oldData, float newData, uint16_t x, uint16_t y, int32_t w, int32_t h, uint8_t decimal, uint16_t colour, uint32_t bg)
{
    //  compareDataOnScreen(oldData, newData);
    // tft.setFreeFont(&Andromeda_Bold24pt7b);

    switch (screen)
    {
    case VMax:
        tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Vol Max
        tft.setTextColor(colour);
        tft.drawString("V", _tx, _ty);
        tft.setSwapBytes(true);
        tft.pushImage(5, 22, 28, 15, GArrow_Up_28x15);

        tft.setFreeFont(&Andromeda_Bold_Tiny_20); // set font for numberic

        if (oldData == newData)
        {
            tft.fillRect(x, y, w, h, bg);
            tft.setTextColor(colour);
            tft.drawFloat(oldData, decimal, x, y);
        }
        break;

    case VNow:
        tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Vol Normal
        tft.setTextColor(colour);
        tft.drawString("V", _tx, _ty);
        tft.setSwapBytes(true);
        tft.pushImage(0, 75, 34, 26, Accu_34x26);

        tft.setFreeFont(&Andromeda_Bold_Tiny_25); // set font for numberic

        if (oldData == newData)
        {
            tft.fillRect(x, y, w, h, bg);
            tft.setTextColor(colour);
            tft.drawFloat(oldData, decimal, x, y);
        }
        break;

    case VMin:
        tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Vol Min
        tft.setTextColor(colour);
        tft.drawString("V", _tx, _ty);
        tft.setSwapBytes(true);
        tft.pushImage(5, 145, 28, 15, Arrow_Down_28x15);

        tft.setFreeFont(&Andromeda_Bold_Tiny_20); // set font for numberic

        if (oldData == newData)
        {
            tft.fillRect(x, y, w, h, bg);
            tft.setTextColor(colour);
            tft.drawFloat(oldData, decimal, x, y);
        }
        break;

    case SHT30_TEMP:
        tft.setFreeFont(&Andromeda_Bold_Tiny_10); // SHT30
        tft.setTextColor(colour);
        tft.setSwapBytes(true);
        tft.pushImage(220, 48, 26, 20, Temperature_Yellow_26x20);
        tft.pushImage(175, 45, 14, 24, Temperature_Yellow_14x24);

        if (oldData == newData)
        {
            tft.fillRect(x, y, w, h, bg);
            tft.setTextColor(colour);
            tft.drawNumber(oldData, x, y);
        }
        break;

    case SHT30_HUMI:
        tft.setFreeFont(&Andromeda_Bold_Tiny_10); // SHT30
        tft.setTextColor(colour);
        tft.drawString("%", _tx, _ty);
        tft.setSwapBytes(true);
        tft.pushImage(258, 48, 16, 19, Huminity_16x19);

        if (oldData == newData)
        {
            tft.fillRect(x, y, w, h, bg);
            tft.setTextColor(colour);
            tft.drawNumber(oldData, x, y);
        }
        break;

    case THERMAL_1:
        tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Temp T1
        tft.setTextColor(colour);
        tft.setSwapBytes(true);
        tft.pushImage(275, 85, 38, 29, Temperature_Red_38x29);
        tft.pushImage(170, 87, 36, 23, Engine_RED_36x23);

        if (oldData == newData)
        {
            tft.fillRect(x, y, w, h, bg);
            tft.setTextColor(colour);
            tft.drawNumber(oldData, x, y);
        }
        break;

    case THERMAL_2:
        tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Temp T1
        tft.setTextColor(colour);
        tft.setSwapBytes(true);
        tft.pushImage(275, 133, 38, 29, Temperature_Blue_38x29);
        tft.pushImage(170, 135, 36, 23, Engine_Blue_36x23);

        if (oldData == newData)
        {
            tft.fillRect(x, y, w, h, bg);
            tft.setTextColor(colour);
            tft.drawNumber(oldData, x, y);
        }
        break;

    default:
        break;
    }

    // if (title == "Vmax")
    // {
    //     tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Vol Max
    //     tft.setTextColor(colour);
    //     tft.drawString("V", _tx, _ty);
    //     tft.setSwapBytes(true);
    //     tft.pushImage(5, 22, 28, 15, GArrow_Up_28x15);

    //     tft.setFreeFont(&Andromeda_Bold_Tiny_20); // set font for numberic
    // }

    // if (title == "Vnow")
    // {
    //     tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Vol Normal
    //     tft.setTextColor(colour);
    //     tft.drawString("V", _tx, _ty);
    //     tft.setSwapBytes(true);
    //     tft.pushImage(0, 75, 34, 26, Accu_34x26);

    //     tft.setFreeFont(&Andromeda_Bold_Tiny_25); // set font for numberic
    // }

    // if (title == "Vmin")
    // {
    //     tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Vol Min
    //     tft.setTextColor(colour);
    //     tft.drawString("V", _tx, _ty);
    //     tft.setSwapBytes(true);
    //     tft.pushImage(5, 145, 28, 15, Arrow_Down_28x15);

    //     tft.setFreeFont(&Andromeda_Bold_Tiny_20); // set font for numberic
    // }

    // if (oldData == 0)
    // {
    //     tft.setTextColor(colour, bg);
    //     tft.drawFloat(oldData, decimal, x, y);
    // }

    // if (title == "sht30TempC")
    // {
    //     tft.setFreeFont(&Andromeda_Bold_Tiny_10); // SHT30
    //     tft.setTextColor(colour);
    //     tft.setSwapBytes(true);
    //     tft.pushImage(230, 48, 26, 20, Temperature_Yellow_26x20);
    //     tft.pushImage(170, 45, 14, 24, Temperature_Yellow_14x24);
    // }

    // if (title == "sht30Humi")
    // {
    //     tft.setFreeFont(&Andromeda_Bold_Tiny_10); // SHT30
    //     tft.setTextColor(colour);
    //     tft.drawString("%", _tx, _ty);
    //     tft.setSwapBytes(true);
    //     tft.pushImage(260, 48, 16, 19, Huminity_16x19);
    // }

    // if (title == "thermal1")
    // {
    //     tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Temp T1
    //     tft.setTextColor(colour);
    //     tft.setSwapBytes(true);
    //     tft.pushImage(290, 85, 38, 29, Temperature_Red_38x29);
    //     tft.pushImage(170, 87, 36, 23, Engine_RED_36x23);
    // }

    // if (title == "thermal2")
    // {
    //     tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Temp T1
    //     tft.setTextColor(colour);
    //     tft.setSwapBytes(true);
    //     tft.pushImage(290, 133, 38, 29, Temperature_Blue_38x29);
    //     tft.pushImage(170, 135, 36, 23, Engine_Blue_36x23);
    // }
    // if (oldData == newData)
    // {

    //     // tft.setTextColor(colour, bg);
    //     // tft.drawFloat(oldData, decimal, x, y);

    //     tft.fillRect(x, y, w, h, bg);
    //     tft.setTextColor(colour);
    //     tft.drawFloat(oldData, decimal, x, y);
    // }
}

void TFT_Screen::showCounter(uint32_t hours, uint32_t minutes, uint32_t seconds, int16_t x, int16_t y, uint16_t colour, uint16_t bg)
{
    static int32_t prevMinutes = -1;
    static int32_t preHours = -1;
    static int32_t preSeconds = -1;

    tft.setSwapBytes(true);
    tft.pushImage(168, 3, 28, 33, Timer_28x33);
    tft.setFreeFont(&Andromeda_Bold_Tiny_20); // Timer
    tft.setTextColor(colour);

    // if (seconds != preSeconds)
    // {

    //     if (seconds < 10)
    //     {
    //         if (seconds == 0)
    //             tft.fillRect(270, 0, 25, 35, bg);

    //         tft.drawString("0", 270, 0);
    //         tft.fillRect(295, 0, 25, 35, bg);
    //         tft.drawNumber(seconds, 295, 0);
    //     }
    //     else
    //     {
    //         tft.fillRect(270, 0, 50, 35, bg);
    //         tft.drawNumber(seconds, 270, 0);
    //     }
    // }
    // preSeconds = seconds;

    if (minutes != prevMinutes)
    {

        if (minutes < 10)
        {
            if (minutes == 0)
                tft.fillRect(270, 0, 25, 35, bg);

            tft.drawString("0", 270, 0);
            tft.fillRect(295, 0, 25, 35, bg);
            tft.drawNumber(minutes, 295, 0);
        }
        else
        {
            tft.fillRect(270, 0, 50, 35, bg);
            tft.drawNumber(minutes, 270, 0);
        }
    }
    prevMinutes = minutes;

    tft.drawString(":", 260, 0); // 260 = x + w(25) * numberic(2)

    if (hours != preHours)
    {
        if (hours < 10)
        {
            if (hours == 0)
                tft.fillRect(x, y, 25, 35, bg); // 1 numberic is w = 25, h = 35

            tft.drawString("0", x, y);
            tft.fillRect(x + 25, y, 25, 35, bg); // 1 numberic is w = 25, h = 35
            tft.drawNumber(hours, x + 25, y);
        }
        else
        {
            tft.fillRect(x, y, 50, 35, bg); // 1 numberic is w = 25, h = 35
            tft.drawNumber(hours, x, y);
        }
    }
    preHours = hours;

    // if (minutes != prevMinutes)
    // {
    //     if (minutes < 10)
    //     {
    //          if (minutes == 0)
    //             tft.fillRect(x, y, 25, 35, bg); // 1 numberic is w = 25, h = 35

    //         tft.drawString("0", x, y);
    //         tft.fillRect(x + 25, y, 25, 35, bg); // 1 numberic is w = 25, h = 35
    //         tft.drawNumber(minutes, x + 25, y);
    //     }
    //     else
    //     {
    //         tft.fillRect(x, y, 50, 35, bg); // 1 numberic is w = 25, h = 35
    //         tft.drawNumber(minutes, x, y);
    //     }
    // }
    // prevMinutes = minutes;
}
