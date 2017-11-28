#ifndef _PlayerManager_H_
#define _PlayerManager_H_

#include "CDL_Timer_Handler.h"
#include "Player.h"
#include <map>
#include <deque>
#include "Util.h"
#include "RobotUtil.h"

#define BETNUM 5

typedef struct _Cfg
{
	short type;
	int bankerwincount;			//当莊机器人赢金币数
	int betwincount;			//下注机器人赢金币数
	char starttime[16];			//开始时间
	char endtime[16];			//结束时间
	BYTE starthour;
	BYTE startmin;
	BYTE endhour;
	BYTE endmin;
	BYTE switchbanker;
	BYTE bankernum;
	BYTE switchbetter;
	int carrybankerlow;
	int carrybankerhigh;
	int carrybetterlow;
	int carrybetterhigh;
	short normalrobotnum;
	int carrynormallow;
	int carrynormalhigh;
	_Cfg():type(0), bankerwincount(0), betwincount(0),starthour(0),startmin(0),endhour(0),endmin(0)
	{}
}Cfg;

class Robot_id
{
	public:
		Robot_id():uid(0), hasused(false){};
		virtual ~Robot_id(){};
		int uid;
		bool hasused;
};

class PlayerManager:public CCTimer
{
	private:
		PlayerManager();
	public:
		static PlayerManager* getInstance();
		virtual ~PlayerManager(){};
		virtual int ProcessOnTimerOut();
		int init();
	public:

		int addPlayer(int uid, Player* player);
		void  delPlayer(int uid, bool isOnclose = false);
		Player* getPlayer(int uid);	
	
	public:
		int getRobotUid();
		int delRobotUid(int robotuid);
		int getPlayerSize();

		void calculateAreaBetLimit(int bankeruid);	//正常情况的区域限制计算
		void updateAreaBetLimit(int64_t bankermoney, int bettype, int rate);			//有玩家某区域下注超过额度或者超过比例时重新计算区域限制
		void ResetBetArray();
		bool CheckAreaBetLimit(int bettype, int64_t betcoin);

		int SavePlayerBetRecord(int uid, int bettype, int64_t betmoney, int pc);
		void ClearBetRecord();

	protected:
		std::map<int , Player*> playerMap;
		struct BetArea
		{
			int bettype;
			int continuePlayCount;
			int64_t betmoney;
		};
		std::map<int, std::deque<BetArea> > playerBetRecord;	//玩家前5局下注记录
	
	public:
		Robot_id m_idpool[MAX_ROBOT_NUMBER];

		int64_t areaTotalBetLimit[BETNUM];	//各区域机器人下注总额限制
		int64_t areaTotalBetArray[BETNUM];	//当前各区域机器人总下注金额

	public:
		Cfg cfg;
		bool addbetrobot;

	private:
		 
};
#endif
