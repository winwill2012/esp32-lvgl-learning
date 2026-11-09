#ifndef DOUBAO_TTS_H
#define DOUBAO_TTS_H

#include <Arduino.h>
#include <WebSocketsClient.h>
#include "driver/i2s.h"
#include <freertos/event_groups.h>
#include <vector>

// ===== 豆包语音合成 2.0（WebSocket 双向流式 v3 协议）=====

// 音频参数：和 AudioPlayer 的播放参数保持一致
#define DOUBAO_TTS_SAMPLE_RATE 16000

// 资源 ID：固定用豆包语音合成 2.0（新版控制台 X-Api-Key 鉴权）
// 注意 seed-tts-2.0 只能调用「2.0 音色」，1.0 音色会被服务端拒绝，
// 所以 DoubaoTTS::currentVoiceType() 会把配置里的 1.0 音色自动映射到 2.0 对应音色。
#define DOUBAO_TTS_RESOURCE_ID "seed-tts-2.0"

// 关键下行事件用事件位记录（粘性），避免被后续事件覆盖导致误判超时
#define TTS_EVENT_CONNECTION_STARTED (1 << 0)
#define TTS_EVENT_CONNECTION_FAILED  (1 << 1)
#define TTS_EVENT_SESSION_STARTED    (1 << 2)
#define TTS_EVENT_SESSION_FINISHED   (1 << 3)
#define TTS_EVENT_SESSION_FAILED     (1 << 4)

// v3 协议事件码（上行/下行通用）
enum DoubaoTtsEvent : int32_t {
    TsEventStartConnection = 1,
    TsEventFinishConnection = 2,
    TsEventConnectionStarted = 50,
    TsEventConnectionFailed = 51,
    TsEventConnectionFinished = 52,
    TsEventStartSession = 100,
    TsEventCancelSession = 101,
    TsEventFinishSession = 102,
    TsEventSessionStarted = 150,
    TsEventSessionCanceled = 151,
    TsEventSessionFinished = 152,
    TsEventSessionFailed = 153,
    TsEventTaskRequest = 200,
    TsEventSentenceStart = 350,
    TsEventSentenceEnd = 351,
    TsEventTtsResponse = 352,
};

class DoubaoTTS final : public WebSocketsClient {
public:
    DoubaoTTS();

    void begin();

    void connect();

    void eventCallback(WStype_t type, uint8_t *payload, size_t length);

    void parseResponse(const uint8_t *response, size_t length);

    // 合成一段文本；disconnectAfter 为 true 时顺带关掉连接
    void synth(const String &text, bool disconnectAfter);

private:
    // 当前音色对应的 X-Api-Resource-Id
    static String resolveResourceId();

    // 当前音色 ID（保证是 2.0 音色）
    static String currentVoiceType();

    // 组装一个带 event 的 full client request 帧
    std::vector<uint8_t> buildFrame(int32_t event, const String &json, const char *sessionId);

    String buildStartSessionJson() const;

    String buildTaskRequestJson(const String &text) const;

    bool sendEvent(int32_t event, const String &json, const char *sessionId);

    bool ensureConnected(unsigned long timeoutMs);

    // 建连握手 + 开会话（幂等），可在预热阶段提前做掉，省掉首句的两次往返
    bool ensureSession();

    // 阻塞等待若干事件位中的任意一个（内部会持续 pump loop()），超时返回 0
    EventBits_t waitForEvent(EventBits_t wanted, unsigned long timeoutMs);

    void enqueueAudio(const uint8_t *data, size_t size);

    EventGroupHandle_t _eventGroup = nullptr;

    volatile bool _isConnecting = false;

    // 最近一次收到的事件码，只用于打日志
    volatile int32_t _lastEvent = 0;

    bool _connectionStarted = false;

    // 当前是否已有一个 StartSession 开好的会话（一句话一个会话）
    bool _sessionStarted = false;

    // 建连时实际使用的资源 ID，音色切换后需要重新握手
    String _resourceId;

    String _sessionId;

    uint32_t _totalAudioBytes = 0;
};

#endif
