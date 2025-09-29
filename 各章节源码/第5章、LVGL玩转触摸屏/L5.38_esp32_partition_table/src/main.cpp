#include "Arduino.h"
#include "SPIFFS.h"

void setup()
{
    Serial.begin(9600);
    delay(3000);

    SPIFFS.begin(true);

    Serial.printf("SPIFFSS总空间大小：%d\n", SPIFFS.totalBytes());
    Serial.printf("SPIFFSS已使用高大小：%d\n", SPIFFS.usedBytes());
}

void loop()
{
}
