#ifndef _TableTimer_H
#define _TableTimer_H

#include <time.h>
#include "CDL_Timer_Handler.h"

#define BET_COIN_TIMER	20
#define TABLE_START_TIMER 21
#define TABLE_KICK_TIMER 22
#define TABLE_SEND_TIMER 23

class Table;
class Player;
class TableTimer; 
class CTimer_Handler:public CCTimer
{
	public:
		virtual int ProcessOnTimerOut();
		void SetTimeEventObj(TableTimer * obj, int timeid, int uid = 0);
		void StartTimer(long interval)
		{
			CCTimer::StartTimer(interval*1000);					
		}  
		void StartMinTimer(long interval)
		{
			CCTimer::StartTimer(interval);					
		}  
	private:
		int timeid;
		int uid;
		TableTimer * handler;
};

class TableTimer
{
	public:
		TableTimer(){} ;
		virtual ~TableTimer() {};	
		void init(Table* table);
	//操作超时函数
	public:
		void stopAllTimer();
		void startBetCoinTimer(int uid,int timeout);
		void stopBetCoinTimer();
		void startTableStartTimer(int timeout);
		void stopTableStartTimer();
		void startKickTimer(int timeout);
		void stopKickTimer();
		void startSendCardTimer(int timeout);
		void stopSendCardTimer();
	//发送通知函数
	public:
		int sendBetTimeOut(Player* player, Table* table, Player* timeoutplayer,Player* nextplayer);
		int sendOpenTimeOut(Player* player, Table* table, Player* timeoutplayer,Player* nextplayer);

	private:
		Table* table;
		int ProcessOnTimerOut(int Timerid, int uid);
		//处理用户下注超时
		CTimer_Handler m_BetTimer;
		//处理桌子开始游戏的定时器
		CTimer_Handler m_TableStartTimer;
		//踢出用户的时间
		CTimer_Handler m_TableKickTimer;
	//超时回调函数
	private:
		int BetTimeOut(int uid);
		int TableGameStartTimeOut();
		int TableKickTimeOut();
		int SendCardTimeOut();

		friend class CTimer_Handler;

		
};

#endif
