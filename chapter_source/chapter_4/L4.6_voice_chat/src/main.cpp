#include "Arduino.h"
#include <WiFi.h>
#include "driver/i2s.h"
#include "DoubaoASR.h"
#include "DoubaoTTS.h"

const char *TAG = "main";

#define CHUNK_SIZE 800             // 50ms音频大小
int16_t buffer[CHUNK_SIZE];
size_t bytesRead;

DoubaoTTS tts;
// TODO: 这里的第一个参数修改成你自己的智能体BotId
CozeAgent agent("7518434105567395881", &tts);
DoubaoASR asr(&agent);

void setup() {
    Serial.begin(9600);
    WiFiClass::mode(WIFI_MODE_STA);
    // TODO: 这里的WiFi名称和密码修改成自己的
    WiFi.begin("ChinaNet-GdPt", "19910226");
    ESP_LOGI(TAG, "正在联网");
    while (WiFiClass::status() != WL_CONNECTED) {
        ESP_LOGI(TAG, ".");
        vTaskDelay(1000);
    }
    ESP_LOGI(TAG, "联网成功");
    // 启用tts语音合成任务
    tts.begin();
    // 启动asr语音识别任务
    asr.begin();
}

void loop() {
    if (Serial.available() > 0) {
        Serial.readStringUntil('\n');
        ESP_LOGI(TAG, "开始录音，请说话，持续时间5s...");
        // 录制100次，每次录制50ms的音频
        for (int i = 0; i < 100; i++) {
            const esp_err_t err = i2s_read(MICROPHONE_I2S_NUM,
                                           buffer,
                                           CHUNK_SIZE * sizeof(int16_t), // 每次录取50ms音频
                                           &bytesRead,
                                           portMAX_DELAY);
            if (err == ESP_OK) {
                // 录音成功就开始创建websocket连接，加快语音识别速度
                asr.connect();
                // 录到的音频，直接发往环形缓冲区
                const BaseType_t result = xRingbufferSend(asr.getRingBuffer(), buffer, bytesRead, portMAX_DELAY);
                if (result != pdTRUE) {
                    ESP_LOGE(TAG, "将录音数据发送到RingBuffer失败");
                }
            }
        }
        ESP_LOGI(TAG, "录音结束");
    }
}
