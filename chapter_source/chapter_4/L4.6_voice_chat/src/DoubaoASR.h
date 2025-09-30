#ifndef DOUBAOASR_H
#define DOUBAOASR_H

#include <vector>
#include <Arduino.h>
#include <freertos/ringbuf.h>

#include "CozeAgent.h"
#include "WebSocketsClient.h"

#define MICROPHONE_I2S_NUM             I2S_NUM_1
#define AUDIO_SAMPLE_RATE              16000
#define AUDIO_RECORDING_SAMPLE_NUMBER  160    // 10ms音频样本数
#define MICROPHONE_I2S_BCLK            42
#define MICROPHONE_I2S_LRC             2
#define MICROPHONE_I2S_DOUT            1

#define TASK_COMPLETED_EVENT (1<<0)  // 表示一次语音识别任务结束的时间位

// 默认头部
constexpr byte DoubaoASRDefaultFullClientWsHeader[] = {0x11, 0x10, 0x10, 0x00};
constexpr byte DoubaoASRDefaultAudioOnlyWsHeader[] = {0x11, 0x20, 0x10, 0x00};
constexpr byte DoubaoASRDefaultLastAudioWsHeader[] = {0x11, 0x22, 0x10, 0x00};

class DoubaoASR : public WebSocketsClient {
public:
    DoubaoASR(CozeAgent *agent);

    void begin();

    void asr(uint8_t *buffer, size_t size, bool firstPacket, bool lastPacket);

    void buildFullClientRequest();

    void buildAudioOnlyRequest(uint8_t *audio, size_t size, bool lastPacket);

    void parseResponse(const uint8_t *response);

    void eventCallback(WStype_t type, const uint8_t *payload, size_t length);

    void consumeRingBuffer(void *ptr);

    RingbufHandle_t getRingBuffer() const;

    void connect();

private:
    const char *TAG = "DoubaoASR";
    RingbufHandle_t _ringBuffer = nullptr; // 用来暂存录音的环形缓冲区
    EventGroupHandle_t _eventGroup = nullptr;
    std::vector<uint8_t> _requestBuilder;
    CozeAgent *_cozeAgent;
    volatile bool _isConnecting = false;
};

#endif //DOUBAOASR_H
