#ifndef DOUBAOTTS_H
#define DOUBAOTTS_H

#include "WebSocketsClient.h"

constexpr uint8_t defaultHeader[] = {0x11, 0x10, 0x10, 0x00};

// 用于描述一个从云端返回的音频数据包
struct PlayAudioTask {
    size_t length;
    int16_t *data;
};

class DoubaoTTS : public WebSocketsClient {
public:
    void begin();

    void connect();

    String buildFullClientRequest(const String &text);

    void parseResponse(const uint8_t *response) const;

    void eventCallback(WStype_t type, uint8_t *payload, size_t length) const;

    void tts(const String &text, bool lastPacket);

    void playAudio(void *ptr) const;

private:
    const char *TAG = "DoubaoTTS";
    // 用于保存音频播放任务的队列
    QueueHandle_t playAudioQueue = xQueueCreate(10, sizeof(PlayAudioTask));;

    // 用于表示语音合成任务是否结束的二值信号量，也可以使用EventGroup实现
    SemaphoreHandle_t taskFinished = xSemaphoreCreateBinary();

    volatile bool _isConnecting = false;
};


#endif //DOUBAOTTS_H
