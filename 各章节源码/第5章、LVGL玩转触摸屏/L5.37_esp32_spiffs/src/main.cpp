#include "Arduino.h"
#include "SPIFFS.h"

const char* testFileName = "/test.txt";

void setup()
{
    Serial.begin(9600);

    if (!SPIFFS.begin(true))
    {
        Serial.println("文件系统初始化失败");
        ESP.restart();
    }
    Serial.println("文件系统初始化成功");

    auto file = SPIFFS.open(testFileName, FILE_WRITE, true);
    if (!file)
    {
        Serial.println("文件以写入模式打开失败");
        ESP.restart();
    }
    Serial.println("文件以写入模式打开成功");

    file.println("第1行: xxxxxx");
    file.println("第2行: yyyyyy");
    file.println("第3行: zzzzzz");

    file.flush();
    file.close();


    file = SPIFFS.open(testFileName, FILE_READ, false);
    if (!file)
    {
        Serial.println("文件以读取模式打开失败");
        ESP.restart();
    }
    Serial.println("文件以读取模式打开成功");
    Serial.printf("文件全部内容: %s\n", file.readString().c_str());

    file.seek(0);
    int line = 1;
    while (file.available())
    {
        Serial.printf("第%d行内容: %s\n", line++, file.readStringUntil('\n').c_str());
    }
    file.close();

    file = SPIFFS.open(testFileName, FILE_APPEND, false);
    if (!file)
    {
        Serial.println("文件以追加模式打开失败");
        ESP.restart();
    }
    Serial.println("文件以追加模式打开成功");

    file.println("第4行: aaaaa");
    file.flush();
    file.close();


    file = SPIFFS.open(testFileName, FILE_READ, false);
    if (!file)
    {
        Serial.println("文件以读取模式打开失败");
        ESP.restart();
    }
    Serial.println("文件以读取模式打开成功");
    Serial.printf("文件全部内容: %s\n", file.readString().c_str());

    file = SPIFFS.open("/rose.jpg", FILE_READ, false);
    if (!file)
    {
        Serial.println("文件以读取模式打开失败");
        ESP.restart();
    }
    Serial.println("文件以读取模式打开成功");
    Serial.printf("文件大小: %d\n", file.size());
}

void loop()
{
}
