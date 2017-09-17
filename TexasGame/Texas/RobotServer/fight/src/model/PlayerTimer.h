#ifndef _PlayerTimer_H
#define _PlayerTimer_H

#include <time.h>
#include "CDL_Timer_Handler.h"

#define BET_COIN_TIMER	20
#define LEAVE_TIMER		21
#define ACTIVE_LEAVE_TIMER 22
#define HEART_TIMER		23
#define START_TIMER		24

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
		void startBetCoinTimer(int uid,int timeout);
		void stopBetCoinTimer();
		void startLeaveTimer(int timeout);
		void stopLeaveTimer();
		void startActiveLeaveTimer(int timeout);
		void stopActiveLeaveTimer();
		void startHeartTimer(int uid,int timeout);
		void stopHeartTimer();
		void startStartGameTimer(int timeout);
		void stopStartGameTimer();
	//发送通知函数
	public:

		int64_t switchmoney;
		int switchmoneyup;

	private:
		Player* player;
		int ProcessOnTimerOut(int Timerid, int uid);
		//处理机器人下注超时
		CTimer_Handler m_BetTimer;
		//处理机器人离开
		CTimer_Handler m_LeaveTimer;
		//随机出去
		CTimer_Handler m_ActiveLeaveTimer;
		//心跳时间
		CTimer_Handler m_HeartTimer;
		//心跳时间
		CTimer_Handler m_StartTimer;
	//超时回调函数
	private:
		int BetTimerTimeOut(int uid);
		int LeaveTimerTimeOut();
		int ActiveLeaveTimeOut();
		int HeatTimeOut(int uid);
		int StartTimeOut();
	private:
		int knowProcess(int uid);
		int notWinProcess(int uid);
		int handleProcess(int uid);
		int Round1Op();
		int Round2Op();
		int Round3Op();
		int Round4Op();
		friend class CTimer_Handler;

		
};

#endif
