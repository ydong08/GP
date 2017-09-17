#ifndef _PlayerManager_H_
#define _PlayerManager_H_

#include "CDL_Timer_Handler.h"
#include "Player.h"
#include <map>

const int IDNUM=800;

class PlayerManager
{
	private:
		PlayerManager();
	public:
		static PlayerManager* getInstance();
		virtual ~PlayerManager(){};

		static int productRobot(int tid, short status, short level, short countUser);

	public:

		int addPlayer(int uid, Player* player);
		void  delPlayer(int uid, bool isclose = false);
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
		int					m_nStartID;
		std::vector<bool>	m_allocInfos;	//主要目的是防止分配一次，多次释放
		std::list<int>		m_frees;
};
#endif
