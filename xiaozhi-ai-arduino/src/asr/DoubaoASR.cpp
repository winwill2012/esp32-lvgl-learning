#include "DoubaoASR.h"
#include "Utils.h"
#include "Settings.h"
#include <Arduino.h>
#include <RecordingManager.h>
#include <cstring>
#include <vector>
#include "LvglDisplay.h"
#include "Application.h"
#include "ArduinoJson.h"
#include "GlobalState.h"

static auto TAG = "ASR";

DoubaoASR::DoubaoASR() {
    _eventGroup = xEventGroupCreate();
    _requestBuilder = std::vector<uint8_t>();
    _firstPacket = true;
    // 豆包流式语音识别 2.0：新版控制台用 X-Api-Key 鉴权，只需要 API Key
    // 基础头先设好，每次真正建连前会在 connect() 里补上 X-Api-Request-Id
    setExtraHeaders(("X-Api-Key: " + Settings::getDoubaoApiKey()).c_str());
    beginSSL("openspeech.bytedance.com", 443, "/api/v3/sauc/bigmodel_async");
    onEvent([this](WStype_t type, uint8_t *payload, size_t length) {
        this->eventCallback(type, payload, length);
    });
}

void DoubaoASR::eventCallback(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_PING:
        case WStype_ERROR:
            break;
        case WStype_CONNECTED:
            ESP_LOGD(TAG, "websocket连接成功");
            break;
        case WStype_DISCONNECTED:
            ESP_LOGD(TAG, "websocket断开连接");
            // 连接被服务端断开时也要解锁 recognize() 的等待，否则会一直卡住
            xEventGroupSetBits(_eventGroup, STT_TASK_COMPLETED_EVENT);
            break;
        case WStype_TEXT: {
            break;
        }
        case WStype_BIN:
            parseResponse(payload, length);
            break;
        default:
            break;
    }
}

// 2.0 的 full client request：不再有 app/appid/cluster/token，
// 也没有 reqid、nbest、sequence、workflow 这些旧字段（它们被协议头/请求头取代了）
void DoubaoASR::buildFullClientRequest() {
    JsonDocument doc;

    const JsonObject user = doc["user"].to<JsonObject>();
    user["uid"] = getChipId(nullptr);

    const JsonObject audio = doc["audio"].to<JsonObject>();
    // 2.0 的 format 只接受 pcm / wav / ogg / mp3，PCM 裸流要写 "pcm"（不能写 "raw"）
    audio["format"] = "pcm";
    audio["codec"] = "raw";
    audio["rate"] = AUDIO_SAMPLE_RATE;
    audio["bits"] = 16;
    audio["channel"] = 1;

    const JsonObject request = doc["request"].to<JsonObject>();
    request["model_name"] = "bigmodel";
    request["result_type"] = "full";       // 每包返回累计全文，直接整段替换显示即可
    request["enable_itn"] = true;          // 数字/金额/日期转阿拉伯数字
    request["enable_punc"] = true;         // 加标点
    request["enable_ddc"] = true;          // 语义顺滑，去掉“嗯、那个”之类语气词
    request["show_utterances"] = false;    // 不需要分句级结果，关掉可减小响应包
    // 一定要显式转成 bool，否则会被序列化成数字 1，服务端 proto 会报 invalid value for bool type
    request["enable_nonstream"] = static_cast<bool>(DOUBAO_ASR_ENABLE_NONSTREAM);
    // VAD 静音判停阈值（毫秒），范围 [300, 5000]，推荐 800~1000
    request["end_window_size"] = 800;
    // 起始 1s 强制按有声处理，避免刚开口就被判停
    request["force_to_speech_time"] = 1000;

    String payloadStr;
    serializeJson(doc, payloadStr);

    _requestBuilder.clear();
    // 先写入报头（四字节）
    _requestBuilder.insert(_requestBuilder.end(), DoubaoASRFullClientHeader,
                           DoubaoASRFullClientHeader + sizeof(DoubaoASRFullClientHeader));
    // 写入payload长度（四字节，大端）
    const std::vector<uint8_t> payloadSize = uint32ToUint8Array(payloadStr.length());
    _requestBuilder.insert(_requestBuilder.end(), payloadSize.begin(), payloadSize.end());
    // 写入payload内容
    _requestBuilder.insert(_requestBuilder.end(), payloadStr.c_str(), payloadStr.c_str() + payloadStr.length());
}

void DoubaoASR::buildAudioOnlyRequest(uint8_t *audio, const size_t size, const bool lastPacket) {
    _requestBuilder.clear();

    if (lastPacket) {
        // 最后一包：messageType=0b0010, flags=0b0010（负包）
        _requestBuilder.insert(_requestBuilder.end(), DoubaoASRLastAudioHeader,
                               DoubaoASRLastAudioHeader + sizeof(DoubaoASRLastAudioHeader));
    } else {
        // 中间包：messageType=0b0010, flags=0b0000
        _requestBuilder.insert(_requestBuilder.end(), DoubaoASRAudioOnlyHeader,
                               DoubaoASRAudioOnlyHeader + sizeof(DoubaoASRAudioOnlyHeader));
    }

    // 16bit PCM 的字节数必须是偶数。奇数字节（比如 1 字节的“占位包”）会被服务端
    // 判成非法音频，返回 55000000 internal error，这里直接截掉多余的那一个字节。
    size_t sendSize = size;
    if (sendSize % 2 != 0) {
        ESP_LOGW(TAG, "音频长度 %u 不是 16bit PCM 的整数倍，已截断最后一个字节", sendSize);
        sendSize--;
    }

    // 写入payload长度（四字节）
    const std::vector<uint8_t> payloadLength = uint32ToUint8Array(sendSize);
    _requestBuilder.insert(_requestBuilder.end(), payloadLength.begin(), payloadLength.end());
    // 写入payload内容（最后一包允许为空，仅靠帧头标记结束）
    if (sendSize > 0 && audio != nullptr) {
        _requestBuilder.insert(_requestBuilder.end(), audio, audio + sendSize);
    }
}

void DoubaoASR::recognize(DoubaoASRTask task) {
    ESP_LOGD(TAG, "开始语音识别请求: 音频长度：%d, 第一个包：%d, 最后一个包：%d", task.data.size(), task.firstPacket,
             task.lastPacket);
    if (task.firstPacket) {
        xEventGroupClearBits(_eventGroup, STT_TASK_COMPLETED_EVENT);
        _firstPacket = true;
        while (!isConnected()) {
            connect();
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        buildFullClientRequest();
        if (!sendBIN(_requestBuilder.data(), _requestBuilder.size())) {
            ESP_LOGE(TAG, "发送语音识别请求包头失败");
        }
        loop();
    }
    buildAudioOnlyRequest(task.data.data(), task.data.size(), task.lastPacket);
    if (!sendBIN(_requestBuilder.data(), _requestBuilder.size())) {
        ESP_LOGE(TAG, "发送语音识别音频数据包失败");
    }
    loop();
    if (task.lastPacket) {
        // 最多等 10s，避免服务端异常时整条录音任务卡死在这里
        const TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(10000);
        while ((xEventGroupWaitBits(_eventGroup, STT_TASK_COMPLETED_EVENT,
                                    false, true, pdMS_TO_TICKS(1)) & STT_TASK_COMPLETED_EVENT) == 0) {
            if (xTaskGetTickCount() > deadline) {
                ESP_LOGW(TAG, "等待语音识别最终结果超时");
                break;
            }
            loop();
            vTaskDelay(1);
        }
        disconnect();
    }
}

// 把最终文本交给 LLM
void DoubaoASR::publishText(const char *text) {
    const size_t length = strlen(text);
    if (length == 0) {
        return;
    }
    ESP_LOGI(TAG, "语音识别结果: %s", text);
    // 注意要预留 '\0' 的位置，否则 toCharArray(buf, len) 会把最后一个字符截掉
    auto *message = static_cast<char *>(ps_malloc(length + 1));
    if (message == nullptr) {
        ESP_LOGE(TAG, "语音识别结果内存分配失败");
        return;
    }
    memcpy(message, text, length + 1);
    LLMTask task{};
    task.message = message;
    task.length = length;
    Application::llm()->publishTask(task);
}

void DoubaoASR::parseResponse(const uint8_t *response, const size_t length) {
    if (response == nullptr || length < 8) {
        return;
    }

    const uint8_t headerSize = (response[0] & 0x0F) * 4;
    const uint8_t messageType = response[1] >> 4;
    const uint8_t flags = response[1] & 0x0F;
    const uint8_t compression = response[2] & 0x0F;

    if (headerSize < 4 || length < static_cast<size_t>(headerSize) + 4) {
        return;
    }
    if (compression != 0) {
        ESP_LOGE(TAG, "服务端返回了压缩数据，当前未实现解压");
        return;
    }

    const uint8_t *payload = response + headerSize;
    const uint8_t *end = response + length;
    ESP_LOGV(TAG, "收到websocket消息类型: %d, flags: %d", messageType, flags);

    switch (messageType) {
        case 0b1111: {
            // 错误帧：error code(4B) + error message size(4B) + error message
            const uint32_t errorCode = static_cast<uint32_t>(readInt32(payload));
            payload += 4;
            const uint32_t messageLength = static_cast<uint32_t>(readInt32(payload));
            payload += 4;
            if (payload + messageLength > end) {
                ESP_LOGE(TAG, "语音识别失败: 错误码 = %u, 错误信息长度非法", errorCode);
            } else {
                ESP_LOGE(TAG, "语音识别失败: 错误码 = %u, 描述 = %s", errorCode,
                         readString(payload, messageLength).c_str());
            }
            xEventGroupSetBits(_eventGroup, STT_TASK_COMPLETED_EVENT);
            break;
        }
        case 0b1001: {
            // 服务端识别结果。flags 低两位决定 header 之后有没有 4 字节 sequence：
            //   0b0000 无 sequence  0b0001 有（正）  0b0010 最后一包（无）  0b0011 最后一包（负）
            const bool lastPacket = (flags & 0b0010) != 0;
            if ((flags & 0b0001) != 0) {
                if (payload + 4 > end) {
                    return;
                }
                const int32_t sequence = readInt32(payload);
                payload += 4;
                ESP_LOGV(TAG, "响应 sequence = %d", sequence);
            }

            if (payload + 4 > end) {
                return;
            }
            const uint32_t payloadSize = static_cast<uint32_t>(readInt32(payload));
            payload += 4;
            if (payloadSize == 0 || payload + payloadSize > end) {
                ESP_LOGW(TAG, "响应 payload 长度非法: %u", payloadSize);
                return;
            }

            JsonDocument jsonResult;
            const DeserializationError err = deserializeJson(
                    jsonResult, reinterpret_cast<const char *>(payload), payloadSize);
            if (err) {
                ESP_LOGE(TAG, "解析语音识别结果失败: %s", err.c_str());
                return;
            }

            // 失败也可能以 0b1001 帧下发，用 code 判断（0 / 20000000 表示成功）
            const int32_t code = jsonResult["code"] | 0;
            if (code != 0 && code != 20000000) {
                ESP_LOGE(TAG, "语音识别失败: code = %d, message = %s", code,
                         static_cast<const char *>(jsonResult["message"] | ""));
                xEventGroupSetBits(_eventGroup, STT_TASK_COMPLETED_EVENT);
                return;
            }

            // 2.0 的识别文本在 result.text，是本次会话已识别的累计全文（result_type=full）
            const char *text = jsonResult["result"]["text"] | "";
            if (text[0] != '\0') {
                LvglDisplay::updateChatText(_firstPacket, text);
                _firstPacket = false;
            }

            if (lastPacket) {
                if (text[0] != '\0') {
                    publishText(text);
                } else if (GlobalState::getState() != Listening) {
                    // 没识别出任何内容（纯静音/噪声），直接回休眠
                    GlobalState::setState(Sleep);
                }
                xEventGroupSetBits(_eventGroup, STT_TASK_COMPLETED_EVENT);
            }
            break;
        }
        default: {
            // 0b1011(server ack) 等类型不需要处理
            ESP_LOGV(TAG, "忽略 websocket 消息类型: %d", messageType);
            break;
        }
    }
}

// 创建websocket连接
void DoubaoASR::connect() {
    if (isConnected() || _isConnecting) return;
    _isConnecting = true;

    // 每次新建连接都刷新鉴权头：2.0 必须带 X-Api-Resource-Id，X-Api-Request-Id 建议每个任务一个随机 ID
    const String headers = "X-Api-Key: " + String(Settings::getDoubaoApiKey().c_str()) +
                           "\r\nX-Api-Resource-Id: " DOUBAO_ASR_RESOURCE_ID +
                           "\r\nX-Api-Request-Id: " + String(generateTaskId().c_str());
    setExtraHeaders(headers.c_str());

    xTaskCreate([](void *args) {
        auto *self = static_cast<DoubaoASR *>(args);
        while (!self->isConnected()) {
            self->loop();
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        self->_isConnecting = false;
        vTaskDelete(nullptr);
    }, "DoubaoAsrConnect", 4096, this, 1, nullptr);
}
