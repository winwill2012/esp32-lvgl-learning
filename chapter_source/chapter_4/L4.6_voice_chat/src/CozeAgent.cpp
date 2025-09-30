#include "CozeAgent.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <utility>

#include "utils.h"

CozeAgent::CozeAgent(String botId, DoubaoTTS *tts) {
    _botId = std::move(botId);
    _tts = tts;
    _stateTransferRouterMap = {
            {std::make_pair(Init, NormalChar),             Init},
            {std::make_pair(Init, Delimiter),              CommandCompleted},

            {std::make_pair(CommandCompleted, NormalChar), CommandCompleted},
            {std::make_pair(CommandCompleted, Delimiter),  ParamsCompleted},

            {std::make_pair(ParamsCompleted, NormalChar),  ParamsCompleted},
            {std::make_pair(ParamsCompleted, Delimiter),   ResponseCompleted}
    };
}

void CozeAgent::reset() {
    _command = "";
    _params = "";
    _response = "";
    _ttsBuffer = "";
    _state = Init;
}

String CozeAgent::getConversationId() {
    return _conversationId;
}

String CozeAgent::getBotId() {
    return _botId;
}

void CozeAgent::chat(const String &query) {
    reset();
    ESP_LOGI(TAG, "发起对话: %s", query.c_str());
    HTTPClient http;
    http.begin("https://api.coze.cn/v3/chat?conversation_id=" + getConversationId());
    // 这里Bearer后面的token修改成你自己的Coze平台的token，可在https://www.coze.cn/open/oauth/pats这里获取
    http.addHeader("Authorization", "Bearer pat_TWjCVBrZMdB0i3hieuhpWpyiF3hTqrouEGd6f3WAaaQvo0mU4dRjCQN9k3tk9WS4");
    http.addHeader("Content-Type", "application/json");
    JsonDocument requestBody;
    requestBody.clear();
    requestBody["stream"] = true;
    requestBody["bot_id"] = getBotId();
    requestBody["user_id"] = "123";
    const JsonArray additionalMessages = requestBody["additional_messages"].to<JsonArray>();
    const JsonObject message = additionalMessages.add<JsonObject>();
    message["content_type"] = "text";
    message["content"] = query;
    message["role"] = "user";
    String requestBodyStr;
    serializeJson(requestBody, requestBodyStr);
    const int httpResponseCode = http.POST(requestBodyStr.c_str());
    if (httpResponseCode > 0) {
        ESP_LOGI(TAG, "Response code: %d", httpResponseCode);
        // 调用coze智能体时开始创建语音合成连接，加快后续合成速度
        _tts->connect();
        WiFiClient *stream = http.getStreamPtr();
        String line = "";
        String lastEvent;
        String output = "";
        // 持续读取流式输出
        while (stream->connected() || stream->available()) {
            // 等待数据流有新的数据可读
            while (!stream->available()) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            line = stream->readStringUntil('\n');
            if (!line.isEmpty()) {
                // ESP_LOGD(TAG, "%s", line.c_str());
                if (line.startsWith("event:")) {
                    // Coze智能体流式调用已返回完整内容
                    if (lastEvent == "event:conversation.message.delta" &&
                        line == "event:conversation.message.completed") {
                        http.end();
                        ESP_LOGI(TAG, "Coze智能体调用结束");
                        ESP_LOGI(TAG, "command: %s", _command.c_str());
                        ESP_LOGI(TAG, "params: %s", _params.c_str());
                        ESP_LOGI(TAG, "response: %s", _response.c_str());
                        // 如果还有未合成的音频数据，继续合成语音
                        if (!_ttsBuffer.isEmpty()) {
                            _tts->tts(_ttsBuffer, true);
                        }
                        return;
                    }
                    lastEvent = line;
                }
                if (line.startsWith("data:")) {
                    String response = line.substring(5);
                    JsonDocument doc;
                    DeserializationError error = deserializeJson(doc, response);
                    if (error) {
                        ESP_LOGE(TAG, "json反序列化失败: %s", error.c_str());
                        continue;
                    }
                    if (doc["content"].is<String>() && doc["type"] == "answer") {
                        auto content = doc["content"].as<String>();
                        processDelta(content);
                        _conversationId = doc["conversation_id"].as<String>();
                    }
                }
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        ESP_LOGI(TAG, "Coze智能体调用结束");
        http.end();
    }
}

// 执行状态转移
void CozeAgent::stateTransfer(LLMState state, LLMEvent event) {
    const auto it = _stateTransferRouterMap.find(std::make_pair(state, event));
    if (it != _stateTransferRouterMap.end()) {
        _state = it->second;
    }
}

void CozeAgent::appendField(const String &delta) {
    // 根据当前状态，追加对应字段的内容
    switch (_state) {
        case Init:
            _command += delta;
            break;
        case CommandCompleted:
            _params += delta;
            break;
        case ParamsCompleted: {
            _response += delta;
            _ttsBuffer += delta;
            const std::pair<int, size_t> delimiterIndex = findMinIndexOfDelimiter(_ttsBuffer);
            // 如果有语义分隔符
            if (delimiterIndex.first >= 0) {
                // 截取分隔符前面的内容，进行语音合成
                _tts->tts(_ttsBuffer.substring(0, delimiterIndex.first), false);
                // 更新还未语音合成的部分
                _ttsBuffer = _ttsBuffer.substring(delimiterIndex.first + delimiterIndex.second);
            }
        }
            break;
        default:
            break;
    }
}

// 处理增量分片数据
void CozeAgent::processDelta(const String &delta) {
    if (delta.isEmpty()) return;
    ESP_LOGV(TAG, "处理智能体增量消息: %s", delta.c_str());
    // 如果新的分片没有包含分隔符，无需执行状态转移
    const int index = delta.indexOf(DELIMITER);
    if (index < 0) {
        // 根据当前状态，追加对应字段的内容
        appendField(delta);
        return;
    }
    // 截取分隔符左边的部分
    const String leftPart = delta.substring(0, index);
    // 分隔符右边剩余部分（很可能还会包含分隔符）
    const String remainingPart = delta.substring(index + 1);

    // Step1: 追加左半部分
    appendField(leftPart);
    // Step2: 然后执行状态转移
    stateTransfer(_state, Delimiter);
    // Step3: 递归处理剩余部分
    processDelta(remainingPart);
}
