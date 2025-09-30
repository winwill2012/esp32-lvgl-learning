#include <Arduino.h>
#include <WebSocketsClient.h>
#include "driver/i2s.h"
#include "ArduinoJson.h"
#include "Utils.h"

#define MAX98357_I2S_NUM  I2S_NUM_0
#define SAMPLE_RATE       16000
#define MAX98357_DOUT     38
#define MAX98357_LRC      40
#define MAX98357_BCLK     39

/**
 * 使用例程需要修改如下：
 * 1、112~113行，从豆包语音技术控制台获取自己的相关信息替换
 * 2、237行，修改为自己的WiFi配置信息
 * 3、246行，其中的Token换成自己的
 */
auto TAG = "TTS";
constexpr uint8_t defaultHeader[] = {0x11, 0x10, 0x10, 0x00};

// 用于保存音频播放任务的队列
QueueHandle_t playAudioQueue;

// 用于表示语音合成任务是否结束的二值信号量，也可以使用EventGroup实现
SemaphoreHandle_t taskFinished;

// websocket客户端
WebSocketsClient client;

// 用于描述一个从云端返回的音频数据包
struct PlayAudioTask
{
    size_t length;
    int16_t* data;
};

// 用于解析云端下发的语音合成数据包
void parseResponse(const uint8_t* response)
{
    const uint8_t messageType = response[1] >> 4;
    const uint8_t messageTypeSpecificFlags = response[1] & 0x0f;
    const uint8_t* payload = response + 4;

    switch (messageType)
    {
    case 0b1011:
        {
            // 0b1011 - Audio-only server response (ACK).
            if (messageTypeSpecificFlags > 0)
            {
                const auto sequenceNumber = readInt32(payload);
                const auto payloadSize = readInt32(payload + 4);
                if (payloadSize > 0)
                {
                    payload += 8;
                    PlayAudioTask task{};
                    task.length = payloadSize / sizeof(int16_t);
                    task.data = static_cast<int16_t*>(ps_malloc(payloadSize));
                    memcpy(task.data, payload, payloadSize);
                    if (xQueueSend(playAudioQueue, &task, portMAX_DELAY) != pdPASS)
                    {
                        ESP_LOGE(TAG, "发送音频播放任务到队列失败: %d", task.length);
                        free(task.data); // 发送到队列失败，则生产者负责将内存回收
                    }
                }
                if (sequenceNumber < 0)
                {
                    ESP_LOGD(TAG, "语音合成任务结束");
                    xSemaphoreGive(taskFinished);
                }
            }
            break;
        }
    case 0b1111:
        {
            // Error message from server (例如错误的消息类型，不支持的序列化方法等等)
            const uint8_t errorCode = readInt32(payload);
            const uint8_t messageSize = readInt32(payload + 4);
            const unsigned char* errMessage = payload + 8;
            ESP_LOGD(TAG, "语音合成失败, code: %d, 原因: %s", errorCode, String(errMessage, messageSize).c_str());
            xSemaphoreGive(taskFinished);
            break;
        }
    default:
        break;
    }
}

void eventCallback(const WStype_t type, uint8_t* payload, const size_t length)
{
    switch (type)
    {
    case WStype_PING:
    case WStype_ERROR:
    case WStype_CONNECTED:
    case WStype_DISCONNECTED:
    case WStype_TEXT:
        break;
    case WStype_BIN:
        parseResponse(payload);
        break;
    default:
        break;
    }
}

String buildFullClientRequest(const String& text)
{
    JsonDocument params;
    const JsonObject app = params["app"].to<JsonObject>();
    app["appid"] = "4630330133";
    app["token"] = "4YOzBPBOFizGvhWbqZroVA3fTXQbeWOW";
    app["cluster"] = "volcano_tts";

    const JsonObject user = params["user"].to<JsonObject>();
    user["uid"] = getChipId(nullptr);

    const JsonObject audio = params["audio"].to<JsonObject>();
    audio["voice_type"] = "zh_female_wanwanxiaohe_moon_bigtts";
    audio["encoding"] = "pcm";
    audio["rate"] = 16000;
    audio["speed_ratio"] = 1.0;
    audio["loudness_ratio"] = 2;

    const JsonObject request = params["request"].to<JsonObject>();
    request["reqid"] = generateTaskId();
    request["text"] = text;
    request["operation"] = "submit";
    String resStr;
    serializeJson(params, resStr);
    return resStr;
}

void tts(const String& text)
{
    ESP_LOGD(TAG, "开始语音合成: %s", text.c_str());
    // 等待websocket建立连接
    while (!client.isConnected())
    {
        // websocket的连接逻辑在loop函数中，这里持续等待，直到连接建立
        client.loop();
        vTaskDelay(1);
    }
    // 发送语音合成数据包
    const String payloadStr = buildFullClientRequest(text);
    uint8_t payload[payloadStr.length()];
    for (int i = 0; i < payloadStr.length(); i++)
    {
        payload[i] = static_cast<uint8_t>(payloadStr.charAt(i));
    }
    payload[payloadStr.length()] = '\0';

    // 获取数据包长度，转换成4字节数组
    const uint32_t payloadSize = payloadStr.length();
    std::vector<uint8_t> payloadLength = uint32ToUint8Array(payloadSize);

    // 先写入四字节Header，可参考官方文档: https://www.volcengine.com/docs/6561/1257584
    std::vector<uint8_t> clientRequest(defaultHeader, defaultHeader + sizeof(defaultHeader));
    // 再写入4字节数据包长度
    clientRequest.insert(clientRequest.end(), payloadLength.begin(), payloadLength.end());
    // 再写入数据包
    clientRequest.insert(clientRequest.end(), payload, payload + sizeof(payload));

    if (!client.sendBIN(clientRequest.data(), clientRequest.size()))
    {
        ESP_LOGE(TAG, "发送语音合成请求数据包失败: %s", text.c_str());
        xSemaphoreGive(taskFinished);
        return;
    }
    // 持续等待语音合成任务结束
    while (xSemaphoreTake(taskFinished, pdMS_TO_TICKS(1)) == pdFALSE)
    {
        client.loop(); // 持续调用loop函数接收云端下发的数据包，直到收到最后一个包任务结束
        vTaskDelay(1);
    }
    client.disconnect();
}

// 用于消费音频播放任务队列，从队列取出音频数据，通过I2S播放
void playAudio(void* ptr)
{
    PlayAudioTask task{};
    size_t bytesWritten;
    while (true)
    {
        // 持续从队列取出播放任务
        if (xQueueReceive(playAudioQueue, &task, portMAX_DELAY) == pdPASS)
        {
            // 写入I2S完成播放
            const esp_err_t result = i2s_write(MAX98357_I2S_NUM,
                                               task.data,
                                               task.length * sizeof(int16_t),
                                               &bytesWritten,
                                               portMAX_DELAY);
            // 播放完成记得释放内存（内存是生产者申请的，消费者处理完要释放）
            free(task.data);
            if (result != ESP_OK)
            {
                ESP_LOGE(TAG, "Play audio failed, errorCode: %d", result);
            }
        }
        vTaskDelay(1);
    }
}

void setup()
{
    Serial.begin(115200);
    constexpr i2s_config_t max98357_i2s_config = {
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // 中断优先级，如果对实时性要求高，可以调高优先级
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .tx_desc_auto_clear = true
    };

    constexpr i2s_pin_config_t max98357_gpio_config = {
        .bck_io_num = MAX98357_BCLK,
        .ws_io_num = MAX98357_LRC,
        .data_out_num = MAX98357_DOUT,
        .data_in_num = -1
    };

    i2s_driver_install(MAX98357_I2S_NUM, &max98357_i2s_config, 0, nullptr);
    i2s_set_pin(MAX98357_I2S_NUM, &max98357_gpio_config);

    taskFinished = xSemaphoreCreateBinary();
    playAudioQueue = xQueueCreate(10, sizeof(PlayAudioTask));

    WiFiClass::mode(WIFI_MODE_STA);
    // 请更换成你自己的wifi账号和密码
    WiFi.begin("ChinaNet-GdPt", "19910226");
    ESP_LOGI(TAG, "正在联网");
    while (WiFiClass::status() != WL_CONNECTED)
    {
        ESP_LOGI(TAG, ".");
        vTaskDelay(1000);
    }
    ESP_LOGI(TAG, "联网成功");

    client.setExtraHeaders("Authorization: Bearer; 4YOzBPBOFizGvhWbqZroVA3fTXQbeWOW");
    client.beginSSL("openspeech.bytedance.com", 443, "/api/v1/tts/ws_binary");
    client.onEvent(eventCallback);

    xTaskCreate(playAudio, "playAudio", 4096, nullptr, 1, nullptr);
}


void loop()
{
    if (Serial.available())
    {
        Serial.readStringUntil('\n');
        ESP_LOGI(TAG, "开始语音合成");
        tts("暮色漫过青瓦时，檐角铜铃晃出细碎的光。风掠过老槐树的年轮，把去年夹在诗集里的银杏叶吹成黄蝶，"
            "扑簌簌落进旧藤椅的褶皱 —— 那里还留着你晒暖的温度。露水在草尖凝结成星子，远处炊烟正牵着暮色往山坳里走，"
            "像极了你走时系在篱笆上的蓝布条，在记忆里飘成一弯瘦月。");
    }
}
