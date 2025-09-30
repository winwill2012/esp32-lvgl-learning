#ifndef COZEAGENT_H
#define COZEAGENT_H

#include <Arduino.h>
#include <DoubaoTTS.h>
#include <map>

// 定义智能体返回的多个字段的分隔符
#define DELIMITER "|"

enum LLMState {
    Init, // 初始化状态
    CommandCompleted, // 命令已接收完成
    ParamsCompleted, // 参数已接收完成
    ResponseCompleted // 回复内容已接收完成
};

enum LLMEvent {
    NormalChar, // 接收到普通字符
    Delimiter, // 遇到字段分隔符
};

class CozeAgent {
public:
    CozeAgent(String botId, DoubaoTTS *tts);

    void reset();

    void chat(const String &query);

    void stateTransfer(LLMState state, LLMEvent event);

    void processDelta(const String &delta);

    void appendField(const String &delta);

    String getConversationId();

    String getBotId();

private:
    const char *TAG = "CozeAgent";
    // 状态转移路由
    std::map<std::pair<LLMState, LLMEvent>, LLMState> _stateTransferRouterMap;
    DoubaoTTS *_tts;
    String _command;
    String _params;
    String _response;
    String _ttsBuffer;
    LLMState _state = Init;
    String _conversationId = "";
    String _botId;
};


#endif //COZEAGENT_H
