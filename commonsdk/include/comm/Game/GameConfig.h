#ifndef __GAME_CONFIG_H_
#define __GAME_CONFIG_H_

#include <string>
#include "RedisAccess.h"
#include "Typedef.h"

enum E_REDIS_TYPE{
    READ_REDIS_CONF  = 0,
    WRITE_REDIS_CONF = 1,
    MAX_REDIS_CONF   = 2,
};

enum E_TRUMPT_LEVEL{
    E_TRUMPT_LEVEL_1 = 0,
    E_TRUMPT_LEVEL_2,
    E_TRUMPT_LEVEL_3,
    E_TRUMPT_LEVEL_4,
    E_TRUMPT_LEVEL_END,
};

enum E_CONFIG_TYPE {
    E_CONFIG_ALLOC,
    E_CONFIG_GAME,
    E_CONFIG_ROBOT,
	E_CONFIG_COMMON
};

class IniFile;

class GameConfig
{
public:
    GameConfig(int gameid, int source, E_CONFIG_TYPE eConfigType = E_CONFIG_GAME);
    virtual ~GameConfig();

public:
    /**
     * 读取命令行参数
     * @param argc
     * @param argv
     * @return
     */
    bool ParseArgs(int argc, char *argv[]);
	/**
	 * 读取游戏相关配置
	 * @return
	 */
	bool LoadConfig(int argc, char *argv[], bool reload = false);
	/**
	 * 游戏服务器实现此接口，用于读取与本游戏相关的配置
	 * @return
	 */
	virtual bool LoadGameConfig() = 0;
	/**
	 * 获取最大桌子数
	 * @return
	 */
	virtual int GetMaxTableNumber() { return 1; }
	/**
	 * 获取最大游戏人数
	 * @return
	 */
	virtual int GetMaxUserNumber() { return 1; }

	/**
	 * 读取游戏专属string键值，szKey会自动加GameTag为前缀
	 * @param szKey
	 * @param szSubKey
	 * @param szDefault
	 * @return
	 */
    std::string ReadString(const char *szKey, const char* szSubKey, const char* szDefault = "");
    /**
     * 读取游戏专属int键值，szKey会自动加GameTag为前缀
     * @param szKey
     * @param szSubKey
     * @param iDefault
     * @return
     */
	int ReadInt(const char *szKey, const char* szSubKey, int iDefault = 0);
	/**
	 * 读取游戏专属double键值，szKey会自动加GameTag为前缀
	 * @param szKey
	 * @param szSubKey
	 * @param dDefault
	 * @return
	 */
	double ReadDouble(const char *szKey, const char* szSubKey, double dDefault = 0);
    /**
     * 读取原始string键值，szKey为原值
     * @param szKey
     * @param szSubKey
     * @param szDefault
     * @return
     */
    std::string ReadRawString(const char *szKey, const char* szSubKey, const char* szDefault = "");
	/**
	* 根据游戏id获取游戏中文名
	* @param gameid
	* @param name
	* @return
	*/
	static void getGameName(int gameid, std::string& name);
	/**
	* 根据游戏level获取对应场次中文名
	* @param gameid
	* @param level
	* @param name
	* @return
	*/
	static void getGameLevelName(int gameid, int level, std::string& name);

	/**
	 * 获取游戏tag
	 */
	const char *GetGameTag() { return m_szGameTag; }

	/// <summary>
	/// 获取游戏中文名
	/// </summary>
	/// <returns></returns>
	const char* GetGameName() { return m_szGameName; }

	/// <summary>
	/// 获取游戏等级中文描述名,如0.1元场等
	/// </summary>
	/// <returns></returns>
	const char* GetLevelName() { return m_szLevelName.c_str(); }

public:
    E_CONFIG_TYPE m_eConfigType;
    std::string m_sLogAddr;
	int    m_nLogLevel;
	int    m_nLogMaxFileSize;
	bool   m_bLogRedirectStdout;

	std::string m_sRoundAddr;

	std::string m_sTrumptAddr;

	std::string m_sDirtyAddr;

	std::string m_sMySQLAddr;

	std::string m_sUserAddr;

	std::string m_sMoneyAddr;

	/**
	 * server config, Offline info, room ocnfig etc，default prot 6380, now change to 6386
	 */
	std::string m_sServerRedisAddr[MAX_REDIS_CONF];
	/**
	 * C++写，php读，排行榜统计,抽税, default port 6381
	 */
	std::string m_sTaxRankRedisAddr[MAX_REDIS_CONF];
	/**
	 * other info redis(OTE), default port 6382
	 */
    std::string m_sOTERedisAddr[MAX_REDIS_CONF];
    /**
     * User config, UserServer专用，default port 6383
     */
    std::string m_sUserRedisAddr[MAX_REDIS_CONF];     //
	/**
	 * user config(UCNF), user detail information,defualt port 6384
	 */
	std::string m_sUCNFRedisAddr[MAX_REDIS_CONF];
    /**
     * money config, money server use it,default port 6385
     */
    std::string m_sMoneyRedisAddr[MAX_REDIS_CONF];
	/**
	* php config, hall and robot use it for crypt redis,default port 6379
	*/
	std::string m_sPhpRedisAddr[MAX_REDIS_CONF];

    
    int32_t     m_nTrumptCoin[E_TRUMPT_LEVEL_END];    //赢取金币小喇叭发送
    
    std::string m_sListenIp;
    uint16_t    m_nPort;
    int32_t     m_nServerId;       //服务器ID
    int32_t     m_nWeight;         //权重
    int32_t     m_nLevel;          //游戏等级
	int 		m_nGameID;			//游戏ID
	short 		m_nSource;			//源ID
	int 		m_nLimitUpRate;			//到达上限，通知大厅
	int 		m_nLimitDownRate;		//低于下限，通知大厅
	int			m_nRobotIndex;		//用于机器人
	int			m_nAllowSameIp;		//是否允许同ip进房间 1 - 允许 0 - 不允许

private:
    const char *m_szGameTag;
	const char* m_szGameName;
	std::string m_szLevelName;
    RedisAccess m_Redis;
};

#endif //End for __GAME_CONFIG_H_
