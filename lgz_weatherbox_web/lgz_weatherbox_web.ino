#include "DHTesp.h"
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Ticker.h>
#include "website.h"

// Display Objects
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
// Sensors  Objects
DHTesp dht;
const byte dhtPin = 17;
// WiFi & WebServer
const char *ssid = "FBK_the_cutest_fox";
const char *password = "zsfv3210";
WebServer server(80);
unsigned int ip[4];
// Values
float pm25 = 0;
double temp = 0, humi = 0;
// MultiProcessing
Ticker ticker01;

// WebServer Functions
void handle_notFound() {
    server.send(404, "text/html; charset=UTF-8", "404 Not Found");
}

void handle_api_data() {
    char buffer[256] = {0};
    snprintf(buffer, sizeof(buffer), "{\"temperature\": \"%.2f\", \"humidity\": \"%.2f\", \"pm25\": \"%.2f\"}", temp, humi, pm25);

    server.send(200, "text/html; charset=UTF-8", buffer);
}

void handle_index() {
    server.send(200, "text/html; charset=UTF-8", HTML);
}

void handleServer() {
    server.handleClient();
}

void setup() {
    // Open U8g2
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_6x13_tr);

    // DHT Setup
    dht.setup(dhtPin, DHTesp::DHT11);

    // Serials
    Serial.begin(115200);
    Serial2.begin(2400);

    // WiFi
    WiFi.begin(ssid, password);
    
    u8g2.firstPage();
    do {
        u8g2.drawStr(0, 15, "Connecting...");
    } while (u8g2.nextPage());
    while (WiFi.status() != WL_CONNECTED);
    for (int i = 0; i < 4; i++) {
        ip[i] = WiFi.localIP()[i];
    }

    // WebServer
    server.onNotFound(handle_notFound);
    server.on("/api/data", handle_api_data);
    server.on("/", handle_index);
    server.begin();

    // MultiProcessing
    ticker01.attach_ms(1, handleServer);
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
    TempAndHumidity lastValues = dht.getTempAndHumidity();
    temp = lastValues.temperature;
    humi = lastValues.humidity;

    pm25 = GetPM25Data();
    // Serial.printf("%.2fmg/m3\n", pm25);   //输出PM2.5的值

    char buf[16] = {0};

    u8g2.firstPage();
    u8g2.setFont(u8g2_font_6x13_tr);
    do {
        u8g2.setCursor(0, 15);
        u8g2.printf("Live On %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

        u8g2.setCursor(0, 30);
        u8g2.printf("Temperature: %.1f C", temp);
        u8g2.setCursor(0, 45);
        u8g2.printf("Humidity: %.1f", humi);

        u8g2.setCursor(0, 60);
        u8g2.printf("PM2.5: %.2f mg/m3", pm25);
    } while (u8g2.nextPage());

    delay(1000);
}