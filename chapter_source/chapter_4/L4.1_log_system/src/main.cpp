#include <Arduino.h>

const char* TAG = "ASR";

void setup()
{
    // Serial.begin(9600);
    // UART0 115200
}

void loop()
{
    Serial.println("Hello World");

    ESP_LOGV(TAG, "这是一条VERBOSE日志: %d", millis());
    ESP_LOGD(TAG, "这是一条DEBUG日志: %d", millis());
    ESP_LOGI(TAG, "这是一条INFO日志: %d", millis());
    ESP_LOGW(TAG, "这是一条WARN日志: %d", millis());
    ESP_LOGE(TAG, "这是一条ERROR日志: %d", millis());
}
