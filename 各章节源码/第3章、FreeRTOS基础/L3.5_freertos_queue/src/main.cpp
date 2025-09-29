#include <Arduino.h>
#include <queue.h>

// 定义订单结构体
typedef struct
{
    int customerId; // 顾客ID
    String dishName; // 菜品名称
    bool isVip; // 是否是VIP
    unsigned long orderTime; // 下单时间
} Order;

// 创建队列
QueueHandle_t orderQueue;

// 顾客下单任务
TaskHandle_t customerTaskHandle;
// 厨师做菜任务
TaskHandle_t chefTaskHandle;

// 菜品列表
const char* dishes[] = {
    "宫保鸡丁", "鱼香肉丝", "麻婆豆腐", "糖醋排骨", "回锅肉"
};

// 顾客任务 - 生产者
void customerTask(void* pvParameters)
{
    Order newOrder;
    int customerCount = 0;

    while (true)
    {
        // 随机生成订单
        newOrder.customerId = ++customerCount;
        newOrder.dishName = dishes[random(0, 5)];
        newOrder.isVip = (random(0, 10) >= 7); // 30%概率是VIP
        newOrder.orderTime = millis();

        Serial.print("顾客 #");
        Serial.print(newOrder.customerId);
        Serial.print(" 点了: ");
        Serial.print(newOrder.dishName);
        Serial.print(", VIP: ");
        Serial.println(newOrder.isVip ? "是" : "否");

        // 根据是否VIP决定放入队列的位置
        if (newOrder.isVip)
        {
            xQueueSendToFront(orderQueue, &newOrder, portMAX_DELAY);
            Serial.println("VIP订单已优先处理");
        }
        else
        {
            xQueueSendToBack(orderQueue, &newOrder, portMAX_DELAY);
            Serial.println("普通订单已加入队列");
        }

        // 等待下一位顾客 (1-5秒)
        vTaskDelay(pdMS_TO_TICKS(random(1000, 5000)));
    }
}

// 厨师任务 - 消费者
void chefTask(void* pvParameters)
{
    Order currentOrder;

    while (true)
    {
        // 从队列接收订单
        if (xQueueReceive(orderQueue, &currentOrder, portMAX_DELAY) == pdPASS)
        {
            Serial.print("厨师开始制作: ");
            Serial.print(currentOrder.dishName);
            Serial.print(" 给顾客 #");
            Serial.println(currentOrder.customerId);

            // 模拟做菜时间 (2-4秒)
            vTaskDelay(pdMS_TO_TICKS(random(2000, 4000)));

            Serial.print("厨师完成了顾客 #");
            Serial.print(currentOrder.customerId);
            Serial.print(" 点的: ");
            Serial.println(currentOrder.dishName);
        }
    }
}

void setup()
{
    Serial.begin(9600);
    randomSeed(millis());
    // 创建订单队列，最多存储10个订单
    orderQueue = xQueueCreate(10, sizeof(Order));
    if (orderQueue != nullptr)
    {
        // 创建任务
        xTaskCreate(customerTask, "Customer", 2048, nullptr, 1, &customerTaskHandle);
        xTaskCreate(chefTask, "Chef", 2048, nullptr, 2, &chefTaskHandle);
    }
    else
    {
        Serial.println("创建队列失败!");
    }
}

void loop()
{
}
