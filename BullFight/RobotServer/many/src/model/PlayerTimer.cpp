#include "PlayerTimer.h"
#include "IProcess.h"
#include "Player.h"
#include "Logger.h"
//====================CTimer_Handler==================================//

int CTimer_Handler::ProcessOnTimerOut()
{
	if(handler)
		return handler->ProcessOnTimerOut(this->timeid, this->uid);
	else
		return 0;

}

void CTimer_Handler::SetTimeEventObj(PlayerTimer * obj, int timeid, int uid)
{
	this->handler = obj;
	this->timeid = timeid;
	this->uid = uid;
}


//==========================PlayerTimer==================================//

void PlayerTimer::init(Player* player)
{
	this->player = player;
}


void PlayerTimer::stopAllTimer()
{
	stopBetCoinTimer();
	stopLeaveTimer();
	stopActiveLeaveTimer();
	stopHeartTimer();
}

void PlayerTimer::startBetCoinTimer(int timeout)
{
	m_BetTimer.SetTimeEventObj(this, BET_COIN_TIMER);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopBetCoinTimer()
{
	m_BetTimer.StopTimer();
}

void PlayerTimer::startLeaveTimer(int timeout)
{
	m_LeaveTimer.SetTimeEventObj(this, LEAVE_TIMER);
	m_LeaveTimer.StartTimer(timeout);
}

void PlayerTimer::stopLeaveTimer()
{
	m_LeaveTimer.StopTimer();
}

void PlayerTimer::startActiveLeaveTimer(int timeout)
{
	m_ActiveLeaveTimer.SetTimeEventObj(this, ACTIVE_LEAVE_TIMER);
	m_ActiveLeaveTimer.StartTimer(timeout);
}

void PlayerTimer::stopActiveLeaveTimer()
{
	m_ActiveLeaveTimer.StopTimer();
}

void PlayerTimer::startHeartTimer(int timeout)
{
	m_HeartTimer.SetTimeEventObj(this, HEART_TIMER);
	m_HeartTimer.StartTimer(timeout);
}

void PlayerTimer::stopHeartTimer()
{
	m_HeartTimer.StopTimer();
}

int PlayerTimer::ProcessOnTimerOut(int Timerid, int uid)
{
	switch (Timerid)
	{
	case BET_COIN_TIMER:
		return BetTimerTimeOut();
	case LEAVE_TIMER:
		return LeaveTimerTimeOut();
	case ACTIVE_LEAVE_TIMER:
		return ActiveLeaveTimeOut();
	case HEART_TIMER:
		return HeatTimeOut();
	default:
		return 0;
	}
	return 0;
}

int PlayerTimer::BetTimerTimeOut()
{
	stopBetCoinTimer();
	if(this->player->betNum < 10)
	{
		//if(Configure::getInstance()->m_nLevel == 3)
		this->startBetCoinTimer(1 + rand() % 3);
		this->player->betCoin();
	}
	return 0;
}

int PlayerTimer::LeaveTimerTimeOut()
{
	this->stopLeaveTimer();
	player->logout();
	return 0;
}

int PlayerTimer::ActiveLeaveTimeOut()
{
	return 0;
}

int PlayerTimer::HeatTimeOut()
{
	this->stopHeartTimer();
	player->heartbeat();
	this->startHeartTimer(30);
	return 0;
}
