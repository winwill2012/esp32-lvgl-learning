#include <Arduino.h>
#include "driver/i2s.h"
#include "SPIFFS.h"

#define MAX98357_I2S_NUM  I2S_NUM_0 // 使用哪个I2S口
#define SAMPLE_RATE       16000  // 音频采样率
#define MAX98357_DOUT     38  // max98357引脚，请参考：https://www.yuque.com/welinklab/pbihut/sdnm396nt3rmcfne
#define MAX98357_LRC      40
#define MAX98357_BCLK     39

File audioFile;
constexpr size_t CHUNK_SIZE = 256;
uint8_t buffer[CHUNK_SIZE];
size_t bytesWritten, bytesRead;

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

    // 启动串口
    Serial.begin(9600);

    if (!SPIFFS.begin(true)) // 挂载文件系统
    {
        Serial.println("SPIFFS Mount Failed");
        ESP.restart();
    }
    audioFile = SPIFFS.open("/test.pcm"); // 打开文件系统中的文件（务必要先上传）
    if (!audioFile)
    {
        Serial.println("File open failed");
        ESP.restart();
    }

    while (audioFile.available()) // 循环读取音频文件内容
    {
        // 对于读到的每一个分片，直接通过i2s_write函数写入I2S通道，最后通过数字功放模块播放出来
        bytesRead = audioFile.read(buffer, CHUNK_SIZE);
        const esp_err_t err = i2s_write(MAX98357_I2S_NUM, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
        if (err != ESP_OK || bytesWritten != bytesRead)
        {
            Serial.println("i2s_write failed");
        }
    }
}

void loop()
{
}
