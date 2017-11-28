
/////////////////////////////////////////////////////////////////////////
// 发起连接管理基类
/////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "CDLSocketHandler.h"
#include "CDL_Timer_Handler.h"

class InputPacket;

class OutputPacket;

class AgentHandler;

class HeartBeatTimer;

class ReconnectTimer;

class BaseAgent;

typedef void(*OnConnectedCallback)(BaseAgent *agent);

typedef void(*OnRegisterRespCallback)(BaseAgent *agent);

class BaseAgent
{
public:
    BaseAgent();

    virtual ~BaseAgent();

    virtual bool init();

    virtual bool init(const std::string &ip, uint16_t port);

    int Send(InputPacket *inputPacket);

    int Send(OutputPacket *outputPacket);

    CDLSocketHandler *GetHandler();

    virtual int OnPacketComplete(const char *data, int len);

    static int registerServerInfo(BaseAgent *agent, const std::string &addres,
                                  int svrid, uint8_t level, int gameid, int weight, int maxuser, int maxtable);

    static int sendHeartBeat(BaseAgent *agent, int serverid);

    void SetRegisterRespCallback(OnRegisterRespCallback cb) { m_cbOnRegisterResp = cb; }

    void SetConnectedCallback(OnConnectedCallback cb) { m_cbOnConnected = cb; }

protected:
    virtual int OnConnected();

    virtual int OnClose();

    virtual int sendHeartBeat();

    /**
     * 处理响应packet函数
     * @return 1表示已处理，0表示未处理
     */
    virtual int responsePacket() = 0;

    virtual int ProcessOnTimerOut();

    virtual bool connect();

    virtual bool reconnect();

    virtual int registerInfo(int maxuser, int maxtable);

    virtual void calculateInterval();    //重新计算重连时间间隔

    virtual void onRegisterResp();

    virtual void onHeartBeatResp();

protected:
    std::string m_sIp;            //要连接的目标ip及端口
    uint16_t m_nPort;
    int64_t m_nInterval;            //重连定时器时间间隔
    bool m_bConnected;            //是否连接成功
    bool m_bFixedReconnect;    //是否固定时间间隔重连
    int m_nReconnectCount;

    std::string m_sName;

    InputPacket *m_pPacket;

    friend class AgentHandler;

    AgentHandler *m_pAgentHandler;

    friend class HeartBeatTimer;

    HeartBeatTimer *m_timerHeartBeat;

    friend class ReconnectTimer;

    ReconnectTimer *m_timerReconnect;

    OnConnectedCallback m_cbOnConnected;
    OnRegisterRespCallback m_cbOnRegisterResp;
};
