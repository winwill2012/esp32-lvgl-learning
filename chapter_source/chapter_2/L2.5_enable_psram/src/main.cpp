#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    delay(5000);
    Serial.print("总剩余内存大小: ");
    Serial.println(esp_get_free_heap_size());

    Serial.print("PSRAM剩余内存大小: ");
    Serial.println(heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    Serial.print("Flash总大小: ");
    Serial.println(ESP.getFlashChipSize());
}

void loop() {
}
