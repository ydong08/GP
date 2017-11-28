#include <json/json.h>
#include "Player.h"
#include "Configure.h"
#include "AllocSvrConnect.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameApp.h"
#include "MoneyAgent.h"
#include "CoinConf.h"
#include "IProcess.h"
#include "Protocol.h"
#include "CoinConf.h"

int pase_json(Json::Value& value, const char* key, int def)
{
	int v = def;
	try{
		v = value[key].asInt();
	}
	catch(...)
	{
		 v = def;
	}
	return v;
}

std::string pase_json(Json::Value& value, const char* key, std::string def)
{
	std::string  v = def;
	try{
		v = value[key].asString();
	}
	catch(...)
	{
		 v = def;
	}
	return v;
}


void Player::init()
{
	id = 0;
	memset(name, 0, sizeof(name));
	memset(json, 0, sizeof(json));
	m_nStatus = 0;
	m_nTabIndex = -1;
	m_nHallid = 0;
	tid = -1;
	source = 0;
	m_bisCall = 0;
	isonline = true;
	m_bhasOpen = false;
	m_nWin = 0;
	m_nLose = 0;
	m_nRunAway = 0;
	m_nTie = 0;
	isbankerlist =false;
	isCanlcebanker = false;
	m_lWinScore = 0;
	m_lReturnScore = 0;
	tax = 0;
	memset(m_lResultArray, 0, sizeof(m_lResultArray));
	memset(m_lBetArray, 0, sizeof(m_lBetArray));
	m_lTempScore = 0;
	m_nLastBetTime = 0;
	m_seatid = 0;
	memset(continuedPlayCount, 0, sizeof(continuedPlayCount));
}

void Player::reset()
{
	m_lWinScore = 0;
	m_lReturnScore = 0;
	m_bisCall = 0;
	tax = 0;
	memset(m_lBetArray, 0, sizeof(m_lBetArray));
	memset(m_lResultArray, 0, sizeof(m_lResultArray));
	memset(continuedPlayCount, 0, sizeof(continuedPlayCount));
}

void Player::login()
{
	//short clevel = Configure::getInstance()->type;
	isonline = true;

	if(strlen(json)>=2 && json[0]=='{')
	{
		Json::Reader reader;
		Json::Value value;
		if (reader.parse(json, value))
		{   
			this->cid = pase_json(value,"cid",0);
			this->sid = pase_json(value,"sid",0); 
			this->pid = pase_json(value,"pid",0);
			//是机器人则用传过来的战绩
			//if(source == 30)
			//{
				int totalNum = pase_json(value,"totalNum",20);
				int winNum = pase_json(value,"winNum",5);
				string link =  pase_json(value,"picUrl", "http://192.126.114.225/ucenter/data/icon/manphoto.png");
				strcpy(this->headlink, link.c_str());
				this->m_nWin = winNum;
				this->m_nLose = totalNum - winNum;
			//}
		} 
	}
	else
	{
		ULOGGER(E_LOG_ERROR, this->id) << "json parse error:" << json;
	}

	AllocSvrConnect::getInstance()->userEnterGame(this);
	this->setEnterTime(time(NULL));
	this->m_nLastBetTime = time(NULL);
}

bool Player::checkMoney(int owned_money)
{
	// 入场限制
	int min_money = CoinConf::getInstance()->getCoinCfg()->minmoney;
	if (min_money > owned_money)
	{
		return false;
	}

	return true;
}

void Player::accumlatePlayCount(int banker)
{
	if (source == E_MSG_SOURCE_ROBOT)
	{
		return;
	}
	if (banker == this->id)
	{
		memset(continuedPlayCount, 0, sizeof(continuedPlayCount)); //如果本局坐庄，则清0
		return;
	}
	for (int i = 1; i < BETNUM; i++)	
	{
		if (m_lBetArray[i] > 0)
		{
			continuedPlayCount[i]++;
		}
		else
		{
			continuedPlayCount[i] = 0;
		}
	}
}

void Player::leave(bool isSendToUser)
{
	if(isSendToUser)
	{
		AllocSvrConnect::getInstance()->userLeaveGame(this);
	}
	IProcess::UpdateDBActiveTime(this);
	OutputPacket respone;
	respone.Begin(UPDATE_LEAVE_GAME_SERVER);
	respone.WriteInt(this->id);
	respone.End();
	if(this->source != E_MSG_SOURCE_ROBOT)
	{
		if (MoneyServer()->Send(&respone) < 0)
		{
			ULOGGER(E_LOG_ERROR, this->id) << "Send request to MoneyServer Error";
		}
	}
	IProcess::UpdateDBActiveTime(this);
	this->m_nStatus = STATUS_PLAYER_LOGOUT;
	this->tid = -1;
	this->m_nTabIndex = -1;
	memset(continuedPlayCount, 0, sizeof(continuedPlayCount));
}

void Player::enter()
{
	AllocSvrConnect::getInstance()->userUpdateStatus(this, this->m_nStatus);
	this->setEnterTime(time(NULL));
	memset(continuedPlayCount, 0, sizeof(continuedPlayCount));
}

bool Player::notBetCoin()
{
	if(m_lBetArray[0] != 0)
		return false;
	return true;
}

