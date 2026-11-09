#ifndef DOUBAO_ASR_H
#define DOUBAO_ASR_H

#include <Arduino.h>
#include <WebSocketsClient.h>
#include <vector>

#define STT_TASK_COMPLETED_EVENT (1<<1)

// ===== 豆包流式语音识别 2.0（SAUC 双向流式，v3 协议）=====

// X-Api-Resource-Id：
//   2.0 小时版   volc.seedasr.sauc.duration
//   2.0 并发版   volc.seedasr.sauc.concurrent
//   1.0 小时版   volc.bigasr.sauc.duration（历史版本）
#define DOUBAO_ASR_RESOURCE_ID "volc.seedasr.sauc.duration"

// 是否开启二遍识别（enable_nonstream）。
// 开启后一句结束时用非流式模型重新识别，准确率更高，但尾字延迟会变大；
// 只在 2.0 的双向流式优化版（bigmodel_async）上支持。
// 注意必须是 bool：写成 1 会被序列化成数字，服务端 proto 是 bool 字段会报
// "invalid value for bool type: 1"
constexpr bool DOUBAO_ASR_ENABLE_NONSTREAM = true;

// 4 字节协议头：[0]ver|headerSize  [1]messageType|flags  [2]serialization|compression  [3]reserved
// full client request：messageType=0b0001, flags=0b0000, JSON, 无压缩
constexpr byte DoubaoASRFullClientHeader[] = {0x11, 0x10, 0x10, 0x00};
// audio only request：messageType=0b0010, flags=0b0000, 无序列化(raw), 无压缩
constexpr byte DoubaoASRAudioOnlyHeader[] = {0x11, 0x20, 0x00, 0x00};
// audio only request + flags=0b0010：最后一包（负包，不带 sequence number）
constexpr byte DoubaoASRLastAudioHeader[] = {0x11, 0x22, 0x00, 0x00};

struct DoubaoASRTask {
    DoubaoASRTask(const std::vector<uint8_t> &data, const bool firstPacket, const bool lastPacket)
            : data(data), firstPacket(firstPacket), lastPacket(lastPacket) {
    };

    // 音频数据
    std::vector<uint8_t> data;

    // 是否第一帧数据
    bool firstPacket;

    // 是否最后一帧数据
    bool lastPacket;
};

class DoubaoASR final : public WebSocketsClient {
public:
    DoubaoASR();

    void connect();

    void eventCallback(WStype_t type, uint8_t *payload, size_t length);

    // v3 协议的响应体长度不固定（是否带 sequence 由 flags 决定），必须把长度传进来做边界检查
    void parseResponse(const uint8_t *response, size_t length);

    void buildFullClientRequest();

    void buildAudioOnlyRequest(uint8_t *audio, size_t size, bool lastPacket);

    void recognize(DoubaoASRTask task);

private:
    // 识别结果交给 LLM 处理（会拷贝一份，调用方无需管理内存）
    static void publishText(const char *text);

    EventGroupHandle_t _eventGroup;
    // 显示用：是否在聊天列表里新起一条消息
    bool _firstPacket;
    volatile bool _isConnecting = false;
    std::vector<uint8_t> _requestBuilder;
};

#endif
