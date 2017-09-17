#pragma once

#include "json/json.h"
#include "threadres.h"

enum MoneyChangeMod
{
    MONEY_GAME = 1 ,          //    1 游戏金币更改  
    MONEY_TAX ,               //    2 台费更改  
    MONEY_PAY ,               //    3 充值   
    MONEY_WITHDRAW ,          //    4 提现  
    MONEY_REGISTE ,           //    5 注册 
    MONEY_RECEIVE_BENEFITS ,  //    6 救济金  
    MONEY_BANK,               //    7 保险箱  
    MONEY_MANAGEMENT,         //    8 系统金币调整 
    MONEY_BING_PHONE,         //    9绑定手机送金币
};

namespace RedisLogic
{
	bool test();

	//用户信息散列
	RedisAccess* GetRdMinfo(int mid=0);
	//公用redis
	RedisAccess* GetRdCommon(const char* mode = "master");
	//公用redis
	RedisAccess* GetRdGame();
	//Game redis  sever专用
	RedisAccess* GetRdServer();
	//账号 专用
	RedisAccess* GetRdAccount(const char* mode = "master");
	//存用户的账号，游戏牌局等非核心资料
	RedisAccess* GetRdOte(int uid, const char* mode = "master");
	//排行榜
	RedisAccess* GetRdRank(int gameid);
	//用户服务器redis
	RedisAccess* GetRdUserServer(int32_t uid, const char* mode = "master");
	//新添加的redis
	RedisAccess* GetRdUcenter(const char* mode = "master");
	//新添加的redis
	RedisAccess* GetRdMoney(const char* mode = "master");


//	bool SetRegAliPayInfo(int uid, CBindAliPayInfo& info);
//	bool GetRegAliPayInfo(int uid, CBindAliPayInfo& info);
	int setMidWithCacheKey(std::string& cacheKey , int mid , bool v1 = false , bool v2 = false);
	int getMidByCacheKey(std::string& cacheKey);

	int doUserInfo(Json::Value& user);

	void getSingleByGameid(Json::Value& user);
	int getLoginReward(int days);

	int doTodayFirst(Json::Value& user);
	void unsetUserFiled(Json::Value& jvUser);
	std::string setMtkey(int32_t mid , std::string& dev_no, std::string& ip);
	int codeCheck(std::string phoneno);
	bool setCodeCheck(std::string phoneno, int idcode);
	bool GetUserInfo(int32_t nMid, Json::Value& info);
	bool SetUserInfo(int32_t nMid, Json::Value& info, uint32_t uTimeout);
	//获取服务器存储的短信验证码
	int getServerIdCode(const std::string& phoneno);
	/**
	 * 每天的次数(累加器)限制.比如每天领奖的次数
	 * @param int mid 用户ID
	 * @param int ltype 计数器类型: 10发feed ...请调用者加到这里
	 * @param int lcount 要加的数量. 如果是0则只获取
	 * @param bool update 是否要加加,默认为true,有时候只想获得当前的数量
	 * @return int 当前的最新值.注意如果没有正确取到缓存则返回一个最大值(如果从来没设置过也是)
	 */
	int limitCount(std::string mid, std::string type, int lcount, bool update=true,int expire=86400);

    bool addWin(const Json::Value& info , int money , int mode , int wflag , const char* desc_ = "");
	bool addNewWin(const std::string& dealNo, int gameid, int mid, int mode, int sid, int pid, int ctype, int wflag, int money, const std::string& desc = "", int type = 10);
    int getProductRoomConfig(int product, int version, Json::Value& out);
    
    void getServerInfo(int gameid, int product, Json::Value& out);
    void getYuleInfo(Json::Value& out);

	bool SetAuthKey(int64_t nMid, const std::string& strAuthKey);

	int getRoomConfigKey(int gameid, std::string& key, std::string& many);
	// 判断是否是维护时间，如果是，再判断是不是可以登录的国家
	bool MaintenanceCheck(const char* szClientIp);
	// 判断该用户是不是快速执行钱的动作，比如存取保险箱，快速提款
	bool IsFastCtrlMoney(int32_t nMid);
	// 获取官方微信，包括购买房卡，招募，举报三个微信号
	void getOfficialWechat(Json::Value& out);
	// 获取玩家房卡信息
	void getUserRoomCardInfo(Json::Value& out);
};

