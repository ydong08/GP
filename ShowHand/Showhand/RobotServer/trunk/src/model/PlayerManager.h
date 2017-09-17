#ifndef _PlayerManager_H_
#define _PlayerManager_H_

#include "CDL_Timer_Handler.h"
#include "Player.h"
#include <map>

const int IDNUM=800;

class Robot_ID
{
	public:
		Robot_ID():uid(0), hasused(false){};
		virtual ~Robot_ID(){};
		int uid;
		bool hasused;
};

class PlayerManager
{
	private:
		PlayerManager();
	public:
		static PlayerManager* getInstance();
		virtual ~PlayerManager(){};
		int init();
	public:

		int addPlayer(int uid, Player* player);
		void  delPlayer(int uid);
		Player* getPlayer(int uid);	
	//获取机器人的ID
	public:
		int getRobotUID();
		int delRobotUid(int robotuid);
		int getPlayerSize();
	protected:
		std::map<int , Player*> playerMap;
	//机器人使用ID池
	public:
		Robot_ID idbuff[IDNUM];
		 
};
#endif
