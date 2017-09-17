//
// Created by new on 2016/10/19.
//

#ifndef DD_SERVERS_GAMEAPP_H
#define DD_SERVERS_GAMEAPP_H

#include "GameConfig.h"
#include "RedisAccess.h"
#include "MoneyAgent.h"
#include "MySqlAgent.h"
#include "TrumptAgent.h"
#include "RoundAgent.h"

class BaseCoinConf;
class CDL_TCP_Server;

class GameApp {
public:
    GameApp();
    ~GameApp();
    /**
     * 注册组件，初始化所有配置
     * @param gameConfig
     * @param coinConf
     */
    void Register(GameConfig* gameConfig, BaseCoinConf* coinConf);

    bool InitConfig(int argc, char *argv[]);
    bool InitAgent();
    int RunLoop();

    virtual bool InitRoom() = 0;

    virtual CDL_TCP_Server* NewTCPServer() = 0;

    GameConfig *GetGameConfig() const {
        return m_pGameConfig;
    }

    BaseCoinConf *GetCoinConf() const {
        return m_pCoinConf;
    }

    RedisAccess *GetOTERedis() const {
        return m_pOTERedis;
    }

    RedisAccess *GetTaxRankRedis() const {
        return m_pTaxRankRedis;
    }

    RedisAccess *GetServerRedis() const {
        return m_pServerRedis;
    }

    RedisAccess *GetUCNFRedis() const {
        return m_pUCNFRedis;
    }

    MoneyAgent *GetMoneyAgent() const {
        return m_pMoneyAgent;
    }

    MySqlAgent *GetMySqlAgent() const {
        return m_pMySqlAgent;
    }

    TrumptAgent *GetTrumptAgent() const {
        return m_pTrumptAgent;
    }

    RoundAgent *GetRoundAgent() const {
        return m_pRoundAgent;
    }
protected:
    GameConfig *m_pGameConfig;
    BaseCoinConf *m_pCoinConf;
    /**
     * redis member
     */
    RedisAccess *m_pOTERedis;
    RedisAccess *m_pTaxRankRedis;
    RedisAccess *m_pServerRedis;
    RedisAccess *m_pUCNFRedis;
    /**
     * agent member
     */
    MoneyAgent  *m_pMoneyAgent;
    MySqlAgent  *m_pMySqlAgent;
    TrumptAgent *m_pTrumptAgent;
    RoundAgent  *m_pRoundAgent;
};

extern GameApp *g_pGameApp;

/**
 * configure macro
 */
#define GameConfigure() g_pGameApp->GetGameConfig()
#define CoinConfigure() g_pGameApp->GetCoinConf()

/**
 * redis macro
 */
#define Ote() g_pGameApp->GetOTERedis()
#define Rank() g_pGameApp->GetTaxRankRedis()
#define Tax() g_pGameApp->GetTaxRankRedis()
#define Server() g_pGameApp->GetServerRedis()
#define Offline() g_pGameApp->GetServerRedis()
#define Round() g_pGameApp->GetOTERedis()
#define UCNF() g_pGameApp->GetUCNFRedis()

/**
 * backserver agent macro
 */
#define MoneyServer() g_pGameApp->GetMoneyAgent()
#define MySqlServer() g_pGameApp->GetMySqlAgent()
#define TrumptServer() g_pGameApp->GetTrumptAgent()
#define RoundServer() g_pGameApp->GetRoundAgent()

/**
 * Source,GameID
 */
#define GAME_ID  (GameConfigure()->m_nGameID)
#define SOURCE  (GameConfigure()->m_nSource)

/**
 * 此模板函数用于返回CDL_TCP_Server对象指针
 * @return
 */
template <typename GameAcceptor>
GameAcceptor* NewGameAcceptor() {
    return new GameAcceptor(GameConfigure()->m_sListenIp.c_str(), GameConfigure()->m_nPort);
}
#endif //DD_SERVERS_GAMEAPP_H
