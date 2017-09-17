#ifndef _PlayerTimer_H
#define _PlayerTimer_H

#include <time.h>
#include "CDL_Timer_Handler.h"

#define CALL_TIMER				20
#define LEAVE_TIMER				21
#define OUTCARD_TIMER			22
#define HEART_TIMER				23
#define OPEN_TIMER				24

#define CHANGECARD_TIMER		25
#define	DEFINELACK_TIMER		26

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
		void startCallTimer(int uid,int timeout);
		void stopCallTimer();
		void startLeaveTimer(int timeout);
		void stopLeaveTimer();
		void startHeartTimer(int uid,int timeout);
		void stopHeartTimer();
		void startOutCardTimer(int uid,int timeout);
		void stopOutCardTimer();
		void startOpenCardTimer(int uid,int timeout);
		void stopOpenCardTimer();

		void startChangeCardTimer(int uid, int timeout);
		void stopChangeCardTimer();

		void startDefineLackTimer(int uid, int timeout);
		void stopDefineLackTimer();

	//发送通知函数
	public:

	private:
		Player* player;
		int ProcessOnTimerOut(int Timerid, int uid);
		//处理机器人下注超时
		CTimer_Handler m_BetTimer;
		//随机出去
		CTimer_Handler m_LeaveTimer;
		//心跳时间
		CTimer_Handler m_HeartTimer;
	//超时回调函数
	private:
		int LeaveTimerTimeOut();
		int HeatTimeOut(int uid);
		int OutCardTimeOut(int uid);
		int OpenTimeOut(int uid);
		int ChangeCardTimeOut(int uid);
		int DefineLackTimeOut(int uid);
	private:
		friend class CTimer_Handler;

		
};

#endif
