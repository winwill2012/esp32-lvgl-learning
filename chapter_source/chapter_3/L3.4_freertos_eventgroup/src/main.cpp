#include <Arduino.h>
#include <event_groups.h>

// 定义洗衣机洗涤过程中的一些事件位
#define DOOR_CLOSED   (1 << 0)  // 表示洗衣机舱门关闭
#define WATER_READY   (1 << 1)  // 表示水位是否满足条件
#define POWER_ON      (1 << 2)  // 表示电源已接通

// 创建事件组句柄
EventGroupHandle_t xWashingEventGroup;

// 检测舱门是否关闭的传感器任务
void vDoorSensorTask(void *pvParameters);
// 检测水位是否到达预定位置的传感器任务
void vWaterSensorTask(void *pvParameters);
// 检测电源是否已接通的任务
void vPowerMonitorTask(void *pvParameters);
// 正式开始洗衣的任务
void vWashingMachineTask(void *pvParameters);

void setup() {
  Serial.begin(9600);
  vTaskDelay(pdMS_TO_TICKS(3000));

  // 创建事件组
  xWashingEventGroup = xEventGroupCreate();

  // 创建各个子任务
  xTaskCreate(vDoorSensorTask, "Door", 1024, nullptr, 1, nullptr);
  xTaskCreate(vWaterSensorTask, "Water", 1024, nullptr, 1, nullptr);
  xTaskCreate(vPowerMonitorTask, "Power", 1024, nullptr, 1, nullptr);
  xTaskCreate(vWashingMachineTask, "Washing", 1024, nullptr, 2, nullptr);

  Serial.println("洗衣机已就绪，请准备洗衣...");
}

void loop() {
}

// 模拟舱门传感器检测任务
void vDoorSensorTask(void *pvParameters) {
  while (true) {
    // 模拟门状态变化
    vTaskDelay(pdMS_TO_TICKS(3000));

    // 随机模拟门是否关闭
    if (random(0, 2) == 1) {
      Serial.println("洗衣机门已关闭");
      xEventGroupSetBits(xWashingEventGroup, DOOR_CLOSED);
    } else {
      Serial.println("洗衣机门未关闭，请关闭门");
      xEventGroupClearBits(xWashingEventGroup, DOOR_CLOSED);
    }
  }
}

// 水位传感器任务
void vWaterSensorTask(void *pvParameters) {
  while (true) {
    // 模拟水位状态变化
    vTaskDelay(pdMS_TO_TICKS(5000));

    // 随机模拟水位是否达到要求
    if (random(0, 2) == 1) {
      Serial.println("水位已达到要求");
      xEventGroupSetBits(xWashingEventGroup, WATER_READY);
    } else {
      Serial.println("水位不足，正在进水...");
      xEventGroupClearBits(xWashingEventGroup, WATER_READY);
    }
  }
}

// 电源监控任务
void vPowerMonitorTask(void *pvParameters) {
  while (true) {
    // 模拟电源状态变化
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 随机模拟电源是否接通
    if (random(0, 2) == 1) {
      Serial.println("电源已接通");
      xEventGroupSetBits(xWashingEventGroup, POWER_ON);
    } else {
      Serial.println("电源未接通，请检查电源...");
      xEventGroupClearBits(xWashingEventGroup, POWER_ON);
    }
  }
}

// 洗衣机任务 - 等待所有条件满足后开始工作
void vWashingMachineTask(void *pvParameters) {
  while (true) {
    // 等待所有条件满足（门关闭、水位达标、电源接通）
    constexpr EventBits_t waitBits = (DOOR_CLOSED | WATER_READY | POWER_ON);

    const EventBits_t uxBits = xEventGroupWaitBits(
      xWashingEventGroup, // 需要监听的事件组
      waitBits, // 需要监听哪些事件，多个事件使用按位或运算组合
      pdFALSE, // 满足条件后，不清除等待的事件位
      pdTRUE, // 需要等待满足所有条件才行，为false则表示任意条件达标即可
      portMAX_DELAY // 无限等待
    );

    // 检查是否所有条件都满足
    if ((uxBits & waitBits) == waitBits) {
      Serial.println("=== 开始洗衣程序 ===");
      Serial.println("洗衣模式：标准");
      vTaskDelay(pdMS_TO_TICKS(1000));
      Serial.println("洗涤中...");
      vTaskDelay(pdMS_TO_TICKS(1000));
      Serial.println("漂洗中...");
      vTaskDelay(pdMS_TO_TICKS(1000));
      Serial.println("脱水中...");
      vTaskDelay(pdMS_TO_TICKS(1000));
      Serial.println("洗衣完成！");
      Serial.println("===================");
    }
  }
}
