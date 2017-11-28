#include <json/json.h>
#include "Player.h"
#include "Configure.h"
#include "AllocSvrConnect.h"
#include "Logger.h"
#include "GameCmd.h"
#include "CoinConf.h"
#include "IProcess.h"
#include "GameApp.h"

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

double pase_json(Json::Value& value, const char* key, double def)
{
	double v = def;
	try{
		v = value[key].asDouble();
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
	m_lRewardCoin = 0;
	m_lLoseScore = 0;
	m_lMaxWinMoney = 0;
	m_lMaxCardValue = 0;
	m_nLastBetTime = 0;
	m_seatid = 0;
}

void Player::reset()
{
	m_lWinScore = 0;
	m_lReturnScore = 0;
	m_lLoseScore = 0;
	m_bisCall = 0;
	tax = 0;
	memset(m_lBetArray, 0, sizeof(m_lBetArray));
	memset(m_lResultArray, 0, sizeof(m_lResultArray));
	m_lRewardCoin = 0;
}

void Player::login()
{
	short clevel = Configure::getInstance().m_nLevel;
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
				string link =  pase_json(value,"picUrl", "");
				strcpy(this->headlink, link.c_str());
				double maxwin = pase_json(value,"maxwin", 0.0); 
				double maxvalue = pase_json(value,"maxvalue", 0.0); 
				this->m_lMaxWinMoney = (int64_t)maxwin; 
				this->m_lMaxCardValue = (int64_t)maxvalue; 
				this->m_nWin = winNum;
				this->m_nLose = totalNum - winNum;
			//}
		} 
	}
	else
	{
		_LOG_ERROR_("json parse error [%s]\n", json);   
	}

	AllocSvrConnect::getInstance()->userEnterGame(this);
	this->setEnterTime(time(NULL));
	this->m_nLastBetTime = time(NULL);
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
	if(this->source != 30)
	{
		if(MoneyServer()->Send(&respone) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );
	}
	IProcess::UpdateDBActiveTime(this);
	this->m_nStatus = STATUS_PLAYER_LOGOUT;
	this->tid = -1;
	this->m_nTabIndex = -1;
}

void Player::enter()
{
	AllocSvrConnect::getInstance()->userUpdateStatus(this, this->m_nStatus);
	this->setEnterTime(time(NULL));
}

bool Player::notBetCoin()
{
	if(m_lBetArray[0] != 0)
		return false;
	return true;
}

