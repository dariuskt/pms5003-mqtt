#include "display.h"


U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


void initDisplay()
{
    u8g2.begin();

    u8g2.setDrawColor(1);
    u8g2.setFontMode(0);

    u8g2.drawBox(0,0,128,32);
    u8g2.sendBuffer();

    delay(100);
    u8g2.setContrast(0);
}

void screenRepaint()
{
    u8g2.clearBuffer();

    showWifi(false);
    showMqtt(false);
    showCount(false);

    u8g2.sendBuffer();
}

void showWifi(bool send)
{
    u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2.setCursor(112,16);
    switch (state.wifi) {
        case 1:
            u8g2.write(207); // connecting
            break;
        case 2:
            u8g2.write(247); // wifi connected
            break;
        case 4:
            u8g2.write(121); //wifi failed
            break;
        default:
            u8g2.write(87); // disconected
    }

    if (send) {
        u8g2.sendBuffer();
    }
}

void showMqtt(bool send)
{
    u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2.setCursor(112,32);
    switch (state.mqtt) {
        case 1:
            u8g2.write(124); // connecting
            break;
        case 2:
            u8g2.write(126); // mqtt connected
            break;
        default:
            u8g2.write(241); // disconected
    }

    if (send) {
        u8g2.sendBuffer();
    }
}

void showCount(bool send)
{
    String value;
    u8g2.setFont(u8g2_font_inb30_mn);

    value = String(state.ugm3_pm_02_5, 10);
    u8g2.setCursor(0,30);
    u8g2.print(value);

    if (send) {
        u8g2.sendBuffer();
    }
}

