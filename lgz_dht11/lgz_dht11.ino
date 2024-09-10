#include "DHTesp.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "u8g2_font_2790a9149e437b34fc94c2ea0f1bb357.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

DHTesp dht;
const byte dhtPin = 17;

void setup() {
    u8g2.begin();
    u8g2.enableUTF8Print();

    dht.setup(dhtPin, DHTesp::DHT11);

    Serial.begin(115200);
}

void loop() {
    TempAndHumidity lastValues = dht.getTempAndHumidity();

    char buf[16] = {0};

    u8g2.firstPage();
    u8g2.setFont(u8g2_font_unifont_myfonts);
    do {
        u8g2.setCursor(0, 15);
        u8g2.print("溫度:");
        u8g2.setCursor(0, 30);
        u8g2.print("濕度:");

        snprintf(buf, sizeof(buf), "%.1f", lastValues.temperature);
        strcat(buf, " C");
        u8g2.drawStr(60, 15, buf);

        snprintf(buf, sizeof(buf), "%.1f", lastValues.humidity);
        strcat(buf, " %RH");
        u8g2.drawStr(60, 30, buf);
    } while (u8g2.nextPage());
}