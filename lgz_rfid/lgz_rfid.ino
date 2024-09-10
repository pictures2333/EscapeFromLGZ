#include <SPI.h>
#include <MFRC522.h>
#include <U8g2lib.h>
#include <Wire.h>

#define SS_PIN 5
#define RST_PIN 0

const int ipaddress[4] = {0, 0, 0, 0};

byte nuidPICC[4] = {0, 0, 0, 0};
const byte corr[4] = {187 , 184, 21, 211};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
    //Init Serial USB
    Serial.begin(115200);
    Serial.println(F("Initialize System"));

    //init rfid D8,D5,D6,D7
    SPI.begin();
    rfid.PCD_Init();
    u8g2.begin();

    Serial.print(F("Reader :"));
    rfid.PCD_DumpVersionToSerial();

    u8g2.firstPage();
    u8g2.setFont(u8g2_font_6x13_tr);
    do {
        u8g2.drawStr(0, 15, "Waiting...");
    } while (u8g2.nextPage());
}

int verify(byte *buffer, byte bufferSize) {
    // size check
    if (bufferSize != sizeof(corr)) return 0;

    // nuid check
    for (int i = 0; i < bufferSize; i++) {
        if (corr[i] != buffer[i]) return 0;
    }

    return 1;
}

void display(byte *buffer, byte bufferSize, int s) {
    u8g2.firstPage();
    u8g2.setFont(u8g2_font_6x13_tr);
    do {
        char buf[270] = "NUID: ";
        char buf2[16] = {0};
        for (int i = 0; i < bufferSize; i++) {
            snprintf(buf2, sizeof(buf2), "%d", buffer[i]);
            strcat(buf, buf2);
            strcat(buf, " ");

            // Prevent Overflow
            if (strlen(buf) > 256) {
                strcpy(buf, "** Overflowed **\x00");
                break;
            }
        } 

        u8g2.drawStr(0, 15, buf);

        if (s == 1) {
            u8g2.drawStr(0, 40, "Access Granted!");
        } else {
            u8g2.drawStr(0, 40, "Denied");
        }
    } while (u8g2.nextPage());
}

void readRFID(void ) { /* function readRFID */
    // Read RFID card
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    // Look for new 1 cards
    if ( ! rfid.PICC_IsNewCardPresent())
      return;

    // Verify if the NUID has been readed
    if (  !rfid.PICC_ReadCardSerial())
      return;

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    int s = verify(rfid.uid.uidByte, rfid.uid.size);

    display(rfid.uid.uidByte, rfid.uid.size, s);

    // Halt PICC
    rfid.PICC_HaltA();

    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
}

void loop() {
  readRFID();
}