#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

SemaphoreHandle_t semaphore;

void count(void* pvParameters)
{
    Serial.println("开始报数...");
    for (int i = 1; i <= 10; i++)
    {
        Serial.printf("当前报数：%d\n", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    xSemaphoreGive(semaphore);  // 报数结束释放信号量，这里释放时候，setup函数中才能Take成功
    vTaskDelete(nullptr);
}

void setup()
{
    Serial.begin(9600);
    semaphore = xSemaphoreCreateBinary();

    // 创建一个报数任务，从1一直数到10报数完成
    xTaskCreate(count,
        "count",
        2048,
        nullptr,
        1,
        nullptr);

    // 等待获取信号量（上一个任务报数完成才会释放信号量），获取不到就一直等着
    xSemaphoreTake(semaphore, portMAX_DELAY);
    Serial.println("成功获取信号量");
}

void loop()
{
}
