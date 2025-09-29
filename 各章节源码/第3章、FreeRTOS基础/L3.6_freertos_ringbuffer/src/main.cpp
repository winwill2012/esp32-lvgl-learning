#include <Arduino.h>
#include "driver/i2s.h"
#include "freertos/ringbuf.h"

#define MAX98357_I2S_NUM  I2S_NUM_0 // 使用哪个I2S口
#define SAMPLE_RATE       16000  // 音频采样率
#define MAX98357_DOUT     38  // max98357引脚，请参考：https://www.yuque.com/welinklab/pbihut/sdnm396nt3rmcfne
#define MAX98357_LRC      40
#define MAX98357_BCLK     39

#define MICROPHONE_I2S_NUM             I2S_NUM_1
#define AUDIO_SAMPLE_RATE              16000
#define MICROPHONE_I2S_BCLK            42
#define MICROPHONE_I2S_LRC             2
#define MICROPHONE_I2S_DOUT            1

#define  READ_SAMPLE_COUNT 80000  // 定义录音长度，一共80K样本，对于16K采样率来说，就是录制5s

int16_t buffer[READ_SAMPLE_COUNT];
size_t bytesRead, bytesWritten;

RingbufHandle_t ringBuffer;

void playAudio(void* ptr);

void setup()
{
    // 构建i2s配置结构体
    constexpr i2s_config_t max98357_i2s_config = {
        // I2S往数字功放发送数据，所以是TX模式
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // 16位宽度
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // 左声道播放
        .communication_format = I2S_COMM_FORMAT_STAND_I2S, // 标准I2S协议
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // 中断优先级，如果对实时性要求高，可以调高优先级
        .dma_buf_count = 4, // DMA缓冲区数量
        .dma_buf_len = 1024, // 每一个缓冲区可以保存的音频样本数量，如果值太大，播放音频有延迟，如果值太小，可能导致音频播放卡顿
        .tx_desc_auto_clear = true // 数据传输完成后自动清理DMA描述符，简单方便，并且可以防止内存泄漏或DMA缓冲区溢出
    };

    // 定义max98357相关引脚
    constexpr i2s_pin_config_t max98357_gpio_config = {
        .bck_io_num = MAX98357_BCLK,
        .ws_io_num = MAX98357_LRC,
        .data_out_num = MAX98357_DOUT,
        .data_in_num = -1
    };

    // 启动I2S驱动
    i2s_driver_install(MAX98357_I2S_NUM, &max98357_i2s_config, 0, nullptr);
    // 让相关配置生效
    i2s_set_pin(MAX98357_I2S_NUM, &max98357_gpio_config);


    constexpr i2s_config_t mic_i2s_config = {
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // 这里的左右声道要和电路保持一致
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false
    };
    constexpr i2s_pin_config_t mic_gpio_config = {
        .bck_io_num = MICROPHONE_I2S_BCLK,
        .ws_io_num = MICROPHONE_I2S_LRC,
        .data_out_num = -1,
        .data_in_num = MICROPHONE_I2S_DOUT
    };

    i2s_driver_install(MICROPHONE_I2S_NUM, &mic_i2s_config, 0, nullptr);
    i2s_set_pin(MICROPHONE_I2S_NUM, &mic_gpio_config);

    Serial.begin(9600);

    ringBuffer = xRingbufferCreate(160000 * sizeof(int16_t), RINGBUF_TYPE_BYTEBUF);
    if (ringBuffer == nullptr)
    {
        Serial.println("ringBuffer creation failed");
        ESP.restart();
    }
    // 启动音频播放任务
    xTaskCreate(playAudio, "playAudioTask", 64000, nullptr, 1, nullptr);
}

// 从RingBuffer中获取录制的音频数据，进行音量增益，最后播放
void playAudio(void* ptr)
{
    size_t readItemSize;
    while (true)
    {
        void* buffer = xRingbufferReceive(ringBuffer, &readItemSize, pdMS_TO_TICKS(1000));
        if (buffer != nullptr)
        {
            auto* audioData = static_cast<int16_t*>(buffer);
            for (int i = 0; i < readItemSize; i++)
            {
                // 因为录制的声音音量较少(可能主板录音孔太小有影响)
                // 所以对音频进行增益并限制在有效范围内
                // 注意此处使用的时int32_t来计算，主要是防止溢出
                auto value = static_cast<int32_t>(audioData[i] * 10.0);
                if (value > 32767) value = 32767;
                if (value < -32768) value = -32768;
                audioData[i] = static_cast<int16_t>(value);
            }
            // 通过i2s_write往数字功放I2S通道写入数据，进行音频播放
            i2s_write(MAX98357_I2S_NUM, audioData, bytesRead,
                      &bytesWritten, portMAX_DELAY);
            // 处理完之后，一定要记得归还缓冲区内存
            vRingbufferReturnItem(ringBuffer, buffer);
        }
    }
    vTaskDelete(nullptr);
}

void loop()
{
    if (Serial.available()) // 判断串口是否有数据输入，有输入输入开始录音
    {
        Serial.readStringUntil('\n');
        Serial.println("Recording...");
        // 通过i2s_read函数从I2S通道录制音频
        // 并且保存到buffer数组中，bytesRead为最终读取的字节数
        const esp_err_t err = i2s_read(MICROPHONE_I2S_NUM, buffer,
                                       80000 * sizeof(int16_t),
                                       &bytesRead, portMAX_DELAY);
        if (err != ESP_OK)
        {
            Serial.println("I2S read failed");
        }
        else
        {
            // 将录音数据写入ringBuffer
            xRingbufferSend(ringBuffer, buffer, bytesRead, portMAX_DELAY);
        }
    }
}
