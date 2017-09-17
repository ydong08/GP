#pragma once
#include "RedisAccess.h"

class RedisAccessMgr
{
public:
	RedisAccessMgr() {}
	~RedisAccessMgr();

	static void redis_ping(void* args);
	static void redis_GetWinGoldRank(void* args);
	void Init();

	//用户信息散列
	RedisAccess* minfo(int mid=0);
	//公用redis
	RedisAccess* common(const char* mode = "master");
	//公用redis
	RedisAccess* game();
	//Game redis  sever专用
	RedisAccess* server();
	//账号 专用
	RedisAccess* account(const char* mode = "master");
	//存用户的账号，游戏牌局等非核心资料
	RedisAccess* ote(int uid, const char* mode = "master");
	//排行榜
	RedisAccess* rank(int gameid);
	//用户服务器redis
	RedisAccess* userServer(int32_t uid, const char* mode = "master");
	//新添加的redis
	RedisAccess* ucenter(const char* mode = "master");
	//新添加的redis
	RedisAccess* money(const char* mode = "master");
private:
	std::map <std::string, RedisAccess*> m_redis_access_map;

};

