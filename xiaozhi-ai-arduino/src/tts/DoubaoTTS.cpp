#include "DoubaoTTS.h"
#include "ArduinoJson.h"
#include "Utils.h"
#include "freertos/task.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <vector>
#include "Settings.h"
#include "Application.h"
#include "AudioPlayer.h"
#include "GlobalState.h"

static auto TAG = "TTS";

// 上行帧头（4 字节）：
//   [0] 0x11  protocol version=1, header size=1（4 字节）
//   [1] 0x14  message type=0b0001(full client request), flags=0b0100(带 event)
//   [2] 0x10  serialization=0b0001(JSON), compression=0b0000(不压缩)
//   [3] 0x00  reserved
constexpr uint8_t kVoiceFrameHeader[4] = {0x11, 0x14, 0x10, 0x00};

// 2.0 只能调用 2.0 音色，这里把配置里的 1.0 音色映射到同角色的 2.0 音色
// （名称对应官方「豆包语音合成模型 2.0 音色列表」）
static const std::map<std::string, const char *> kVoice20Map = {
        {"zh_female_wanwanxiaohe_moon_bigtts", "zh_female_xiaohe_uranus_bigtts"},
        {"zh_female_tianmeixiaoyuan_moon_bigtts", "zh_female_tianmeixiaoyuan_uranus_bigtts"},
        {"zh_female_shuangkuaisisi_moon_bigtts", "zh_female_shuangkuaisisi_uranus_bigtts"},
        {"zh_female_linjianvhai_moon_bigtts", "zh_female_linjianvhai_uranus_bigtts"},
        {"zh_female_meilinvyou_moon_bigtts", "zh_female_meilinvyou_uranus_bigtts"},
        {"zh_female_gaolengyujie_moon_bigtts", "zh_female_gaolengyujie_uranus_bigtts"},
        {"zh_female_yuanqinvyou_moon_bigtts", "zh_female_sajiaoxuemei_uranus_bigtts"},
        {"zh_male_shaonianzixin_moon_bigtts", "zh_male_shaonianzixin_uranus_bigtts"},
        {"zh_male_sunwukong_mars_bigtts", "zh_male_sunwukong_uranus_bigtts"},
        {"zh_female_peiqi_mars_bigtts", "zh_female_peiqi_uranus_bigtts"},
};

// 兜底音色（小何 2.0）
static constexpr char kDefaultVoice20[] = "zh_female_xiaohe_uranus_bigtts";

DoubaoTTS::DoubaoTTS() {
    _eventGroup = xEventGroupCreate();
    begin();
}

// 固定用 2.0 资源（新版控制台 X-Api-Key 鉴权）
String DoubaoTTS::resolveResourceId() {
    return DOUBAO_TTS_RESOURCE_ID;
}

String DoubaoTTS::currentVoiceType() {
    const std::map<std::string, std::string> voiceMap = Settings::getVoiceMap();
    const std::string current = Settings::getCurrentVoice().c_str();
    const auto it = voiceMap.find(current);

    // 没配置就退回默认音色
    if (it == voiceMap.end() || it->second.empty()) {
        return kDefaultVoice20;
    }

    // 已经是 2.0 音色，直接用
    if (it->second.find("_uranus_bigtts") != std::string::npos) {
        return it->second.c_str();
    }

    // 1.0 音色 → 映射到 2.0 对应音色
    const auto mapped = kVoice20Map.find(it->second);
    if (mapped != kVoice20Map.end()) {
        return mapped->second;
    }

    ESP_LOGW(TAG, "音色 %s 在 2.0 里没有对应版本，改用默认音色 %s", it->second.c_str(), kDefaultVoice20);
    return kDefaultVoice20;
}

std::vector<uint8_t> DoubaoTTS::buildFrame(const int32_t event, const String &json, const char *sessionId) {
    std::vector<uint8_t> frame;
    frame.reserve(json.length() + 64);

    frame.insert(frame.end(), kVoiceFrameHeader, kVoiceFrameHeader + sizeof(kVoiceFrameHeader));

    // event number（4 字节，大端）
    const auto eventBytes = uint32ToUint8Array(static_cast<uint32_t>(event));
    frame.insert(frame.end(), eventBytes.begin(), eventBytes.end());

    // 会话类事件：session id size + session id
    if (sessionId != nullptr) {
        const size_t sessionIdLength = strlen(sessionId);
        const auto sessionIdLengthBytes = uint32ToUint8Array(sessionIdLength);
        frame.insert(frame.end(), sessionIdLengthBytes.begin(), sessionIdLengthBytes.end());
        frame.insert(frame.end(), sessionId, sessionId + sessionIdLength);
    }

    // payload size + payload
    const auto payloadLengthBytes = uint32ToUint8Array(json.length());
    frame.insert(frame.end(), payloadLengthBytes.begin(), payloadLengthBytes.end());
    frame.insert(frame.end(), json.c_str(), json.c_str() + json.length());
    return frame;
}

String DoubaoTTS::buildStartSessionJson() const {
    JsonDocument doc;
    const JsonObject user = doc["user"].to<JsonObject>();
    user["uid"] = getChipId(nullptr);
    doc["event"] = static_cast<int>(TsEventStartSession);
    doc["namespace"] = "BidirectionalTTS";

    const JsonObject reqParams = doc["req_params"].to<JsonObject>();
    reqParams["speaker"] = currentVoiceType();

    const JsonObject audioParams = reqParams["audio_params"].to<JsonObject>();
    audioParams["format"] = "pcm";
    audioParams["sample_rate"] = DOUBAO_TTS_SAMPLE_RATE;
    // 设置里是 0.5~2.0 的倍率，2.0 接口要的是 [-50, 100] 的百分比（100 = 2 倍速）
    int speechRate = static_cast<int>(lround((Settings::getCurrentSpeakSpeedRatio() - 1.0) * 100.0));
    speechRate = std::max(-50, std::min(100, speechRate));
    audioParams["speech_rate"] = speechRate;
    // 固定用最大音量合成（100 = 2 倍音量），实际音量在播放阶段再调整
    audioParams["loudness_rate"] = 100;

    String json;
    serializeJson(doc, json);
    return json;
}

String DoubaoTTS::buildTaskRequestJson(const String &text) const {
    JsonDocument doc;
    const JsonObject user = doc["user"].to<JsonObject>();
    user["uid"] = getChipId(nullptr);
    doc["event"] = static_cast<int>(TsEventTaskRequest);

    const JsonObject reqParams = doc["req_params"].to<JsonObject>();
    reqParams["speaker"] = currentVoiceType();
    const JsonObject audioParams = reqParams["audio_params"].to<JsonObject>();
    audioParams["format"] = "pcm";
    audioParams["sample_rate"] = DOUBAO_TTS_SAMPLE_RATE;
    reqParams["text"] = text;

    String json;
    serializeJson(doc, json);
    return json;
}

bool DoubaoTTS::sendEvent(const int32_t event, const String &json, const char *sessionId) {
    const std::vector<uint8_t> frame = buildFrame(event, json, sessionId);
    if (!sendBIN(const_cast<uint8_t *>(frame.data()), frame.size())) {
        ESP_LOGE(TAG, "发送事件 %d 失败", event);
        return false;
    }
    return true;
}

bool DoubaoTTS::ensureConnected(const unsigned long timeoutMs) {
    const TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeoutMs);
    while (!isConnected()) {
        connect();
        if (static_cast<int32_t>(xTaskGetTickCount() - deadline) >= 0) {
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return true;
}

EventBits_t DoubaoTTS::waitForEvent(const EventBits_t wanted, const unsigned long timeoutMs) {
    const TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(timeoutMs);
    while (true) {
        const EventBits_t bits = xEventGroupGetBits(_eventGroup) & wanted;
        if (bits != 0) {
            return bits;
        }
        if (static_cast<int32_t>(xTaskGetTickCount() - deadline) >= 0) {
            return 0;
        }
        loop();
        vTaskDelay(1);
    }
}

// 把"建连握手 + 开会话"两趟往返提前做掉（幂等）。
// 由预热阶段调用时，这两趟都在等大模型返回的空档里完成，不占首句的延迟；
// 万一预热没成功，synth() 里再调一次会补上。
bool DoubaoTTS::ensureSession() {
    if (!_connectionStarted) {
        xEventGroupClearBits(_eventGroup, TTS_EVENT_CONNECTION_STARTED | TTS_EVENT_CONNECTION_FAILED);
        if (!sendEvent(TsEventStartConnection, "{}", nullptr)) {
            return false;
        }
        if (!(waitForEvent(TTS_EVENT_CONNECTION_STARTED | TTS_EVENT_CONNECTION_FAILED, 5000)
              & TTS_EVENT_CONNECTION_STARTED)) {
            ESP_LOGE(TAG, "等待 ConnectionStarted 失败，最后事件 = %d", _lastEvent);
            return false;
        }
        _connectionStarted = true;
        ESP_LOGD(TAG, "连接握手完成");
    }

    if (!_sessionStarted) {
        // 服务端同一连接下不支持并发 session，所以一次会话对应一句话
        _sessionId = generateTaskId().c_str();
        xEventGroupClearBits(_eventGroup, TTS_EVENT_SESSION_STARTED | TTS_EVENT_SESSION_FAILED);
        if (!sendEvent(TsEventStartSession, buildStartSessionJson(), _sessionId.c_str())) {
            return false;
        }
        if (!(waitForEvent(TTS_EVENT_SESSION_STARTED | TTS_EVENT_SESSION_FAILED, 10000)
              & TTS_EVENT_SESSION_STARTED)) {
            ESP_LOGE(TAG, "等待 SessionStarted 失败，最后事件 = %d", _lastEvent);
            return false;
        }
        _sessionStarted = true;
        ESP_LOGD(TAG, "会话已创建，等待文本");
    }
    return true;
}

void DoubaoTTS::synth(const String &text, const bool disconnectAfter) {
    if (text.isEmpty()) {
        return;
    }

    const unsigned long startTime = millis();
    ESP_LOGI(TAG, "开始语音合成：%s", text.c_str());

    // 换了音色就可能从 1.0 音色切到 2.0 音色，资源 ID 是握手头，必须重新建连
    if (_connectionStarted && _resourceId != resolveResourceId()) {
        ESP_LOGI(TAG, "音色对应的资源 ID 变化，重新建立连接");
        disconnect();
        _connectionStarted = false;
        _sessionStarted = false;
    }

    if (!ensureConnected(15000)) {
        ESP_LOGE(TAG, "语音合成连接建立失败（HTTP %u）", _client.cCode);
        return;
    }

    // 预热任务还在跑（它在自己的线程里 pump loop），等它结束再动手，避免两个线程同时 loop()
    const TickType_t prewarmDeadline = xTaskGetTickCount() + pdMS_TO_TICKS(8000);
    while (_isConnecting && static_cast<int32_t>(xTaskGetTickCount() - prewarmDeadline) < 0) {
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    if (_isConnecting) {
        // 预热迟迟不结束：这一句放弃，交给下一句重试，避免并发 pump loop()
        ESP_LOGE(TAG, "语音合成会话预热超时，跳过本句");
        return;
    }

    // 正常情况下预热已经把连接握手和会话都建好了，这里直接返回 true
    if (!ensureSession()) {
        return;
    }

    // 发文本 + 声明本会话结束
    xEventGroupClearBits(_eventGroup, TTS_EVENT_SESSION_FINISHED | TTS_EVENT_SESSION_FAILED);
    if (!sendEvent(TsEventTaskRequest, buildTaskRequestJson(text), _sessionId.c_str())) {
        return;
    }
    if (!sendEvent(TsEventFinishSession, "{}", _sessionId.c_str())) {
        return;
    }

    // 等音频合成完（音频在回调里已经陆续交给播放器了）
    const EventBits_t bits = waitForEvent(TTS_EVENT_SESSION_FINISHED | TTS_EVENT_SESSION_FAILED, 30000);
    if (bits & TTS_EVENT_SESSION_FINISHED) {
        ESP_LOGI(TAG, "语音合成完成，音频 %u 字节，耗时 %lu ms", _totalAudioBytes, millis() - startTime);
    } else {
        ESP_LOGE(TAG, "等待 SessionFinished 失败，最后事件 = %d", _lastEvent);
    }
    // 本会话已结束，下一句要重新 StartSession
    _sessionStarted = false;
    _totalAudioBytes = 0;

    if (disconnectAfter) {
        sendEvent(TsEventFinishConnection, "{}", nullptr);
        disconnect();
        _connectionStarted = false;
        _sessionStarted = false;
    }
}

void DoubaoTTS::enqueueAudio(const uint8_t *data, const size_t size) {
    if (data == nullptr || size == 0) {
        return;
    }
    auto *samples = static_cast<int16_t *>(ps_malloc(size));
    if (samples == nullptr) {
        ESP_LOGE(TAG, "音频内存分配失败: %u 字节", size);
        return;
    }
    memcpy(samples, data, size);

    PlayAudioTask task{};
    task.length = size / sizeof(int16_t);
    task.data = samples;
    Application::audioPlayer()->publishTask(task);

    _totalAudioBytes += size;
}

void DoubaoTTS::parseResponse(const uint8_t *response, const size_t length) {
    if (response == nullptr || length < 8) {
        return;
    }

    const uint8_t headerSize = (response[0] & 0x0F) * 4;
    const uint8_t messageType = response[1] >> 4;
    if (headerSize < 4 || length < static_cast<size_t>(headerSize) + 4) {
        return;
    }

    const uint8_t *payload = response + headerSize;
    const uint8_t *end = response + length;

    switch (messageType) {
        case 0b1111: {
            // 错误帧：header + error code(4B) + payload size(4B) + json
            const uint32_t errorCode = static_cast<uint32_t>(readInt32(payload));
            payload += 4;
            if (payload + 4 > end) {
                return;
            }
            const uint32_t messageSize = static_cast<uint32_t>(readInt32(payload));
            payload += 4;
            if (payload + messageSize > end) {
                return;
            }
            ESP_LOGE(TAG, "语音合成失败, code: %u, reason: %s", errorCode,
                     readString(payload, messageSize).c_str());
            _lastEvent = TsEventConnectionFailed;
            xEventGroupSetBits(_eventGroup, TTS_EVENT_CONNECTION_FAILED | TTS_EVENT_SESSION_FAILED);
            break;
        }
        case 0b1001:     // full server response（JSON）
        case 0b1011: {   // audio only response（音频二进制）
            const int32_t event = readInt32(payload);
            payload += 4;
            _lastEvent = event;

            // 关键事件用“粘性”事件位记录，服务端后续的 350/351/352 不会把它冲掉
            switch (event) {
                case TsEventConnectionStarted:
                    xEventGroupSetBits(_eventGroup, TTS_EVENT_CONNECTION_STARTED);
                    break;
                case TsEventConnectionFailed:
                    xEventGroupSetBits(_eventGroup, TTS_EVENT_CONNECTION_FAILED);
                    break;
                case TsEventSessionStarted:
                    xEventGroupSetBits(_eventGroup, TTS_EVENT_SESSION_STARTED);
                    break;
                case TsEventSessionFinished:
                    xEventGroupSetBits(_eventGroup, TTS_EVENT_SESSION_FINISHED);
                    break;
                case TsEventSessionFailed:
                    xEventGroupSetBits(_eventGroup, TTS_EVENT_SESSION_FAILED);
                    break;
                default:
                    break;
            }

            // 连接类事件（50/51/52）后面是 connect id，而且不保证一定带，
            // 我们只需要事件本身（已在上面置位），直接返回，避免误解析出告警
            if (event >= 50 && event <= 52) {
                break;
            }

            // 会话类事件（150~352）在 event 之后多一段 session id
            if (event >= 150 && event <= 352) {
                if (payload + 4 > end) {
                    return;
                }
                const uint32_t sessionIdSize = static_cast<uint32_t>(readInt32(payload));
                payload += 4;
                if (payload + sessionIdSize > end) {
                    return;
                }
                payload += sessionIdSize;
            }

            if (payload + 4 > end) {
                return;
            }
            const uint32_t payloadSize = static_cast<uint32_t>(readInt32(payload));
            payload += 4;
            if (payload + payloadSize > end) {
                ESP_LOGW(TAG, "响应 payload 长度非法: %u", payloadSize);
                return;
            }

            if (messageType == 0b1011 || event == TsEventTtsResponse) {
                enqueueAudio(payload, payloadSize);
                break;
            }

            if (payloadSize == 0) {
                break;
            }

            JsonDocument doc;
            if (deserializeJson(doc, reinterpret_cast<const char *>(payload), payloadSize)) {
                ESP_LOGW(TAG, "响应 JSON 解析失败, event = %d", event);
                break;
            }
            if (event == TsEventSessionFailed) {
                ESP_LOGE(TAG, "会话失败: %s",
                         static_cast<const char *>(doc["message"] | doc["error"] | ""));
            } else if (event == TsEventSessionFinished) {
                ESP_LOGD(TAG, "会话结束，计费字数 = %d", doc["usage"]["text_words"] | 0);
            } else if (event == TsEventSentenceEnd) {
                ESP_LOGV(TAG, "句子合成完成: %s", static_cast<const char *>(doc["text"] | ""));
            }
            break;
        }
        default:
            break;
    }
}

void DoubaoTTS::eventCallback(const WStype_t type, uint8_t *payload, const size_t length) {
    switch (type) {
        case WStype_PING:
            break;
        case WStype_ERROR:
            ESP_LOGW(TAG, "websocket 错误");
            break;
        case WStype_CONNECTED:
            ESP_LOGD(TAG, "websocket 连接成功");
            break;
        case WStype_DISCONNECTED:
            ESP_LOGW(TAG, "websocket 断开连接，HTTP 状态码 = %u", _client.cCode);
            _connectionStarted = false;
            _sessionStarted = false;
            break;
        case WStype_TEXT:
            // 服务端有时用文本帧下发异常信息
            ESP_LOGW(TAG, "收到文本消息: %s", String(reinterpret_cast<char *>(payload), length).c_str());
            break;
        case WStype_BIN:
            parseResponse(payload, length);
            break;
        default:
            break;
    }
}

void DoubaoTTS::begin() {
    beginSSL("openspeech.bytedance.com", 443, "/api/v3/tts/bidirection");
    onEvent([this](const WStype_t type, uint8_t *payload, const size_t length) {
        this->eventCallback(type, payload, length);
    });
}

// 创建 websocket 连接
void DoubaoTTS::connect() {
    if (isConnected() || _isConnecting) return;
    _isConnecting = true;

    // 新版控制台鉴权：X-Api-Key + X-Api-Resource-Id（语音合成 2.0）
    _resourceId = resolveResourceId();
    _client.cCode = 0;

    const String headers = "X-Api-Key: " + String(Settings::getDoubaoApiKey().c_str()) +
                           "\r\nX-Api-Resource-Id: " + _resourceId +
                           "\r\nX-Api-Connect-Id: " + String(generateTaskId().c_str());
    setExtraHeaders(headers.c_str());

    ESP_LOGI(TAG, "建立语音合成连接，资源 ID = %s，音色 = %s，API Key 长度 = %u",
             _resourceId.c_str(), currentVoiceType().c_str(),
             static_cast<unsigned>(Settings::getDoubaoApiKey().size()));

    xTaskCreate([](void *args) {
        auto *self = static_cast<DoubaoTTS *>(args);
        const TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(6000);
        // status: 0=未连接 1=已发握手等响应 2=已连上
        uint8_t lastStatus = self->_client.status;
        while (!self->isConnected()) {
            if (static_cast<int32_t>(xTaskGetTickCount() - deadline) >= 0) {
                break;
            }
            self->loop();
            if (self->_client.status != lastStatus) {
                ESP_LOGD(TAG, "连接状态 %d -> %d", lastStatus, self->_client.status);
                lastStatus = self->_client.status;
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        if (!self->isConnected()) {
            ESP_LOGE(TAG, "语音合成连接失败，HTTP 状态码 = %u（0 表示握手被拒、没拿到响应头）",
                     self->_client.cCode);
            self->_isConnecting = false;
        } else {
            // 连接通了就顺手把 StartConnection / StartSession 也做掉：
            // 这两趟往返本来会压在第一句话上，现在放在等大模型回复的空档里完成
            self->ensureSession();
            self->_isConnecting = false;
        }
        vTaskDelete(nullptr);
    }, "DoubaoTTSConnect", 4096, this, 1, nullptr);
}
