#ifndef _PlayerManager_H_
#define _PlayerManager_H_

#include "CDL_Timer_Handler.h"
#include "RobotUtil.h"
#include "Player.h"
#include "Util.h"
#include <map>

struct RobotID
{
	int  uid;
	bool hasused;
};

class PlayerManager
{
	typedef std::map<int, Player*>  MapPlayer;

	Singleton(PlayerManager);

	public:
		static int productRobot(int tid, short status, short level, short countUser);
		
		int  addPlayer(int uid, Player* player);
		void delPlayer(int uid, bool iscolsed = false);
		Player* getPlayer(int uid);	
	
		int getRobotUID();
		int delRobotUid(int robotuid);
		int getPlayerSize();

	private:
		MapPlayer m_playerMap;
		RobotID   m_idpool[MAX_ROBOT_NUMBER];
};
#endif
