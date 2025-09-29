#include <Arduino.h>

TaskHandle_t printMessageTask1;
TaskHandle_t printMessageTask2;
TaskHandle_t printMessageTask3;
TaskHandle_t printMessageTask4;

void printMessage(void* ptr)
{
    Serial.println(static_cast<char*>(ptr));
    while (true)
    {
        Serial.printf("当前任务参数: %s, 运行CPU: %d\n", static_cast<char*>(ptr), xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    // 这是第一个永久运行的任务，所以该行代码不会被运行，如果是会结束的任务，
    // 则需要在任务的最后，删除当前任务，参数为NULL表示删除自己
    vTaskDelete(nullptr);
}

void setup()
{
    Serial.begin(9600);
    vTaskDelay(pdMS_TO_TICKS(3000));
    xTaskCreate(printMessage, // 任务函数指针
                "printMessage1", // 任务名称
                2048, // 任务堆栈大小，简单的任务堆栈可以设置小一些，复杂的任务需要设置大一点得到堆栈，不然容易堆栈溢出
                const_cast<char*>("这是第1个任务参数"), // 需要传递什么参数到任务处理函数中
                1, // 任务优先级，可选范围为0-24，数字越大，优先级越高
                &printMessageTask1); // 任务句柄，如果后续对该任务有控制需求，可以设置该参数，TaskHandle_t类型

    xTaskCreate(printMessage, // 任务函数指针
                "printMessage2", // 任务名称
                4096, // 任务堆栈大小，简单的任务堆栈可以设置小一些，复杂的任务需要设置大一点得到堆栈，不然容易堆栈溢出
                const_cast<char*>("这是第2个任务参数"), // 需要传递什么参数到任务处理函数中
                2, // 任务优先级，可选范围为0-24，数字越大，优先级越高
                &printMessageTask2); // 任务句柄，如果后续对该任务有控制需求，可以设置该参数，TaskHandle_t类型

    vTaskDelay(pdMS_TO_TICKS(5000));
    // 5s以后，删除以上两个任务
    vTaskDelete(printMessageTask1);
    vTaskDelete(printMessageTask2);

    xTaskCreatePinnedToCore(printMessage,
                            "printMessage3",
                            2048,
                            const_cast<char*>("第3个任务参数"),
                            1,
                            &printMessageTask3,
                            0);
    xTaskCreatePinnedToCore(printMessage,
                            "printMessage4",
                            2048,
                            const_cast<char*>("第4个任务参数"),
                            2,
                            &printMessageTask4,
                            1);
    vTaskDelay(pdMS_TO_TICKS(5000));
    // 5s以后，删除以上两个任务
    vTaskDelete(printMessageTask3);
    vTaskDelete(printMessageTask4);
}

void loop()
{
}
