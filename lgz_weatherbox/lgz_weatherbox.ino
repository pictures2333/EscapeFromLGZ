#include "DHTesp.h"
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

DHTesp dht;
const byte dhtPin = 17;

void setup() {
    u8g2.begin();
    u8g2.enableUTF8Print();

    dht.setup(dhtPin, DHTesp::DHT11);

    Serial.begin(115200);

    Serial2.begin(2400);
}

float GetPM25Data()//讀取PM2.5感測器，鮑率：2400; 檢查位元：無; 停止位元：1 位;資料位元：8; 數據包長度為7位元組
{
    int cnt,pmval,readcmd[7];
    unsigned char gdata,eFlag,rbytes=0; // eFlag 指示資料傳輸狀態的旗標
    float pm25;
    cnt=0;
    eFlag =0;
    while(Serial2.available()>0) {
        gdata = Serial2.read();//保存接收字符

        if(gdata==0xAA && eFlag==0) { eFlag=1 ; };  //起始位是0xAA
        if(eFlag==1) readcmd[rbytes++]=gdata;
        cnt++;
        if(cnt>100) return 0;
        if(rbytes==7)  break; //一共7組數據
    }

    if(rbytes==0) return 0;
    // readcmd [ 0xAA , ? , ? , ? , ? , ? , 0xFF ]
    if(readcmd[6]!=0xFF) return 0;   //結束位元為OxFF
      
    pmval = readcmd[1];
    pmval<<=8;  //左移8位 Vout(H)*256
    pmval+=readcmd[2];  //Vout(H)*256+Vout(L)
    pm25 = pmval*5.0/1024.0;//計算PM2.5值，：Vout=(Vout(H)*256+Vout(L))/1024*5 
    pm25/=3.5;  //係数3.5
    // for (rbytes = 0; rbytes < 7;rbytes++ ){
    //   Serial.println(readcmd[rbytes]);   
    // } 
    // Serial.println();
    return pm25;
}

void loop() {
    float pm25=0;

    TempAndHumidity lastValues = dht.getTempAndHumidity();

    pm25 = GetPM25Data();
    // Serial.printf("%.2fmg/m3\n", pm25);   //输出PM2.5的值

    char buf[16] = {0};

    u8g2.firstPage();
    u8g2.setFont(u8g2_font_6x13_tr);
    do {
        u8g2.drawStr(0, 15, "[* LGZ Weather Box *]");

        u8g2.setCursor(0, 30);
        u8g2.printf("Temperature: %.1f C", lastValues.temperature);
        u8g2.setCursor(0, 45);
        u8g2.printf("Humidity: %.1f", lastValues.humidity);

        u8g2.setCursor(0, 60);
        u8g2.printf("PM2.5: %.2f mg/m3", pm25);
    } while (u8g2.nextPage());

    delay(1000);
}