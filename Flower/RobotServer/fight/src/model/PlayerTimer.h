#ifndef _PlayerTimer_H
#define _PlayerTimer_H

#include <time.h>
#include "CDL_Timer_Handler.h"

#define LEAVE_TIMER				20
#define HEART_TIMER				21
#define START_TIMER				22
#define BET_TIMER				23
#define CHECK_TIMER				24

class Player;
class Player;
class PlayerTimer; 
class CTimer_Handler:public CCTimer
{
	public:
		virtual int ProcessOnTimerOut();
		void SetTimeEventObj(PlayerTimer * obj, int timeid, int uid = 0);
		void StartTimer(long interval)
		{
			CCTimer::StartTimer(interval*1000);					
		}  
	private:
		int timeid;
		int uid;
		PlayerTimer * handler;
};

class PlayerTimer
{
	public:
		PlayerTimer(){} ;
		virtual ~PlayerTimer() {};	
		void init(Player* player);
	//操作超时函数
	public:
		void stopAllTimer();
		void startLeaveTimer(int timeout);
		void stopLeaveTimer();
		void startHeartTimer(int uid,int timeout);
		void stopHeartTimer();
		void startGameStartTimer(int uid,int timeout);
		void stopGameStartTimer();
		void startBetCoinTimer(int uid,int timeout);
		void stopBetCoinTimer();
		void startCheckTimer(int uid,int timeout);
		void stopCheckTimer();
	//发送通知函数
	public:

		int KnowProcess();
		int UnKnowProcess();
	private:
		Player* player;
		int ProcessOnTimerOut(int Timerid, int uid);
		//处理机器人下注超时
		CTimer_Handler m_BetTimer;
		//随机出去
		CTimer_Handler m_LeaveTimer;
		//心跳时间
		CTimer_Handler m_HeartTimer;
		//心跳时间
		CTimer_Handler m_CheckTimer;
	//超时回调函数
	private:
		int LeaveTimerTimeOut();
		int HeatTimeOut(int uid);
		int StartGameTimeOut(int uid);
		int BetTimeOut(int uid);
		int CheckTimeOut(int uid);

	private:
		friend class CTimer_Handler;

		
};

#endif
