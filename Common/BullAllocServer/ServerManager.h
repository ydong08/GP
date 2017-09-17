#ifndef _ServerManager_H_
#define _ServerManager_H_

#include "CDLSocketHandler.h"
#include "HallManager.h"
#include "Protocol.h"
#include "RedisAccess.h"
#include <map>

#define SERVER_STAT_CLOSE -1 // 
#define SERVER_STAT_OPEN 1 // 

#define SERVER_SVID 0 
#define SERVER_PRIORITY 1 
#define SERVER_USER 2 
#define SERVER_TYPE 3 

//extern int GAME_ID;


typedef struct GameServer
{
	short svid;
	//char ip[64];
	std::string ip;
	short port;
	short level;
	short status;
	short priority;
	int maxUserCount;
	int maxTabCount;		 
	int currUserCount;
	int currTabCount;
	int lasttime;

	int IPhoneUser;
	int AndroidUser;
	int IPadUser;
	int RobotUser;

	int gameID; //add by maoshuoqiong 20160818

	GameServer() : svid(0), port(0), level(0), status(0),
		priority(0), maxUserCount(0), maxTabCount(0), currUserCount(0),
		currTabCount(0), lasttime(0), IPadUser(0), IPhoneUser(0), AndroidUser(0), RobotUser(0), gameID(0)
	{

	}
	
}GameServer;


class ServerManager
{
	public:
		static ServerManager* getInstance();
		bool initRedis(const char* ip, int port);
		virtual ~ServerManager() {};
		int addServer(int id,  GameServer* server);
		int delServer(int id);
		int getServer(int id,  GameServer* server, bool isredis=true);
		GameServer* getServer(int id);
		int setServerStatus(int id, int status);
		int setServerPriority(int id, int priority);
		int updateServerMember(int id, GameServer* server);
		int getAllServer(GameServer** p, int* count,int level = 0, bool isredis=false);

		int getGameTypes(int* games);
		int getCurrUserNum(int level, int* basenum);
		//是否从redis中取出最优的，用于区分从缓存中取还是从进程内存中取
		GameServer* getBestServer(int level = 0, bool isredis=false);

		void buildPacketToHall(OutputPacket& ReportPkg);
		
		int addGameServerHandler(int id, CDLSocketHandler* p);
		void delGameServerHandler(int id);
		CDLSocketHandler* getGameServerHandler(int id);
		void addCurrTabCount(int id);
		int getCurrTabCount(int id);
		int sendToGmServer(int svid, OutputPacket* outPack, bool isEncrypt=true);

		//以游戏ID和游戏等级,更新、插入在线人数 add by maoshuoqiong 20160818
		int getCurrUserNum(E_SERVER_TYPE gameid,E_GAME_LEVEL_ID level);
		int updateCurrUserNum(GameServer* server);
	protected:
		std::map<int, CDLSocketHandler*> serversMap;
		std::map<int, GameServer*> _svidsMap;//添加server的信息，主要目的是进程内存一份数据缓存一份数据

	private:
		int setServerProperty(int uid, const char* field, const char* value);
		int setServerProperty(int uid, const char* field, int  value);
		int send(CDLSocketHandler* clientHandler, OutputPacket* outPack, bool isEncrypt=true);

	private:
		ServerManager() {};

		RedisAccess m_redis;
		 
};
#endif
