#include "PlayerTimer.h"
#include "BaseProcess.h"
#include "Player.h"
#include "Logger.h"
#include "Configure.h"
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
	stopLeaveTimer();
	stopCallTimer();
	stopHeartTimer();
}

void PlayerTimer::startCallTimer(int uid,int timeout)
{
	m_BetTimer.SetTimeEventObj(this, CALL_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopCallTimer()
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

void PlayerTimer::startHeartTimer(int uid,int timeout)
{
	m_HeartTimer.SetTimeEventObj(this, HEART_TIMER, uid);
	m_HeartTimer.StartTimer(timeout);
}

void PlayerTimer::stopHeartTimer()
{
	m_HeartTimer.StopTimer();
}

void PlayerTimer::startOutCardTimer(int uid,int timeout)
{
	m_BetTimer.SetTimeEventObj(this, OUTCARD_TIMER, uid);
	m_BetTimer.StartTimer(1);
}

void PlayerTimer::stopOutCardTimer()
{
	m_BetTimer.StopTimer();
}

void PlayerTimer::startOpenCardTimer(int uid,int timeout)
{
	m_BetTimer.SetTimeEventObj(this, OPEN_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopOpenCardTimer()
{
	m_BetTimer.StopTimer();
}

void PlayerTimer::startChangeCardTimer(int uid, int timeout)
{
	m_BetTimer.SetTimeEventObj(this, CHANGECARD_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopChangeCardTimer()
{
	m_BetTimer.StopTimer();
}

void PlayerTimer::startDefineLackTimer(int uid, int timeout)
{
	m_BetTimer.SetTimeEventObj(this, DEFINELACK_TIMER, uid);
	m_BetTimer.StartTimer(timeout);
}

void PlayerTimer::stopDefineLackTimer()
{
	m_BetTimer.StopTimer();
}

int PlayerTimer::ProcessOnTimerOut(int Timerid, int uid)
{
	switch (Timerid)
	{
	case CALL_TIMER:
		return 0;
		//return CallTimeOut(uid);
	case LEAVE_TIMER:
		return LeaveTimerTimeOut();
	case HEART_TIMER:
		return HeatTimeOut(uid);
	case OUTCARD_TIMER:
		return OutCardTimeOut(uid);
	case OPEN_TIMER:
		return OpenTimeOut(uid);
	case CHANGECARD_TIMER:
		return ChangeCardTimeOut(uid);
	case DEFINELACK_TIMER:
		return DefineLackTimeOut(uid);
	default:
		return 0;
	}
	return 0;
}

int PlayerTimer::LeaveTimerTimeOut()
{
	this->stopLeaveTimer();
	ULOGGER(E_LOG_INFO, player->m_Uid) << "stop robot, tid = " << player->m_TableId;
	player->logout();
	return 0;
}

int PlayerTimer::OutCardTimeOut(int uid)
{
	this->stopOutCardTimer();
	if(player->m_Uid != uid)
	{
		ULOGGER(E_LOG_ERROR, player->m_Uid) << " is not this uid" << uid;
		return -1;
	}
	player->Operate();
	return 0;
}

int PlayerTimer::OpenTimeOut(int uid)
{
	this->stopOpenCardTimer();
	if(player->m_Uid != uid)
	{
		ULOGGER(E_LOG_ERROR, player->m_Uid) << " is not this uid" << uid;
		return -1;
	}

	return 0;
}

int PlayerTimer::HeatTimeOut(int uid)
{
	this->stopHeartTimer();
	if(player->m_Uid != uid)
	{
		ULOGGER(E_LOG_ERROR, player->m_Uid) << " is not this uid" << uid;
		return -1;
	}
	player->heartbeat();
	this->startHeartTimer(uid, 15);
	return 0;
}

int PlayerTimer::ChangeCardTimeOut(int uid)
{
	this->stopChangeCardTimer();
	if (player->m_Uid != uid)
	{
		ULOGGER(E_LOG_ERROR, player->m_Uid) << " is not this uid" << uid;
		return -1;
	}
	player->ChangeCard();
	return 0;
}

int PlayerTimer::DefineLackTimeOut(int uid)
{
	this->stopDefineLackTimer();
	if (player->m_Uid != uid)
	{
		ULOGGER(E_LOG_ERROR, player->m_Uid) << " is not this uid" << uid;
		return -1;
	}
	player->DefineLack();
	return 0;
}