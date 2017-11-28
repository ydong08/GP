#include <json/json.h>
#include "Player.h"
#include "AllocSvrConnect.h"
#include "Configure.h"
#include "Logger.h"
#include "CoinConf.h"
#include "IProcess.h"
#include "GameApp.h"
#include "ProtocolServerId.h"
#include "GameCmd.h"

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

bool pase_json_bool(Json::Value& value, const char* key, bool def)
{
	bool  v = def;
	try{
		v = value[key].asBool();
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
	level = 0;
	tid = -1;										//牌座ID 
	m_nTabIndex = -1;								//当前在桌子的位置
	m_lMoney = 0;									//金币数
	m_lCarryMoney = 0;								//这盘牌携带金币数
	m_nWin = 0;										//赢牌数
	m_nLose = 0;									//输牌数
	m_nRunAway = 0;
	m_nTie = 0;
	memset(json, 0, sizeof(json));					//json字符串
	m_nStatus = 0;									//状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
	source = 0;										//用户来源
	m_nType = 0;									//当前金币场底注 0-无底注
	memset(card_array, 0, sizeof(card_array));		//当前手牌
	m_bHasCard = false;
	m_nHallid = 0;									//属于哪个大厅ID

	active_time = 0;
	enter_time = 0;
	memset(betCoinList, 0, sizeof(betCoinList));
	m_lFinallGetCoin = 0;
	m_nEndCardType = -1;
	m_bThisRoundBet = false;
	m_bAllin = false;
	isorder = false;
	memset(&coincfg,0,sizeof(CoinCfg));
	optype = 0;
	isdropline = false;
	cid = 0;
	sid = 0;
	pid = 0;
	timeoutCount = 0;
	startnum = 0;
	m_nRoll = 0;
	memset(cbEndCardData, 0, sizeof(cbEndCardData));
	memset(cbTempCardDate, 0, sizeof(cbTempCardDate));
	memset(&tempWinlist, 0, sizeof(tempWinlist));
	m_lBetLimit = 0;
	m_setPoolResult.clear();
	m_lAddMoney = 0;
	memset(m_PoolNumArray, 0, sizeof(m_PoolNumArray));
	m_bTempCardType = 0;
	m_pTask = NULL;
	m_bCompleteTask = false;
	m_nGetRoll = 0;
	m_nMagicNum = 0;
	m_nMagicCoinCount = 0;

	m_lMaxWinMoney = 0;
	m_lMaxCardValue = 0;
	m_nBuyInFlag = 0;
}

void Player::reset()
{	
	memset(card_array, 0, sizeof(card_array));	//当前手牌
	m_bHasCard = false;
	memset(betCoinList, 0, sizeof(betCoinList));	
	m_lFinallGetCoin = 0;
	m_nEndCardType = -1;
	m_bThisRoundBet = false;
	m_bAllin = false;
	optype = 0;
	isorder = false;
	memset(cbEndCardData, 0, sizeof(cbEndCardData));
	memset(cbTempCardDate, 0, sizeof(cbTempCardDate));
	memset(&tempWinlist, 0, sizeof(tempWinlist));
	m_lBetLimit = 0;
	m_setPoolResult.clear();
	memset(m_PoolNumArray, 0, sizeof(m_PoolNumArray));
	m_bTempCardType = 0;
	m_pTask = NULL;
	m_bCompleteTask = false;
	m_nGetRoll = 0;
	m_nMagicNum = 0;
	m_nMagicCoinCount = 0;
}

void Player::login()
{
	short m_nType = Configure::getInstance()->level;
	/*if (Configure::getInstance()->level == 6 || Configure::getInstance()->level == 7)
	{
		m_nType = 3;
	}*/
	coincfg.level = m_nType;

	if(strlen(json)>=2 && json[0]=='{')
	{
		Json::Reader reader;
		Json::Value value;
		if (reader.parse(json, value))
		{   
			this->cid = pase_json(value,"cid",0);
			this->sid = pase_json(value,"sid",0); 
			this->pid = pase_json(value,"pid",0); 
			int tasknum = pase_json(value,"task",0); 
			int totalNum = pase_json(value, "sum", 0);
			int winNum = pase_json(value, "win", 0);
			this->m_nWin = winNum;
			this->m_nLose = totalNum - winNum;
			double maxwin = pase_json(value, "maxwin", 0.0);
			double maxvalue = pase_json(value, "maxvalue", 0.0);
			this->m_lMaxWinMoney = (int64_t)maxwin;
			this->m_lMaxCardValue = (int64_t)maxvalue;
		} 
	}
	else
	{
		_LOG_ERROR_("json parse error [%s]\n", json);   
	}

	AllocSvrConnect::getInstance()->userEnterGame(this);
	this->setEnterTime(time(NULL));
	//属于第一次
	startnum = 1;

	if(this->source != 30)
	{
		int i ;
	}
}

void Player::leave()
{
	AllocSvrConnect::getInstance()->userLeaveGame(this);
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
	this->m_bHasCard = false;
	this->m_nTabIndex = -1;
}

void Player::enter()
{
	AllocSvrConnect::getInstance()->userUpdateStatus(this, this->m_nStatus);
}

bool Player::checkMoney()
{
	CoinConf* coinCalc = CoinConf::getInstance();
	coinCalc->getCoinCfg(&coincfg);
	if (this->m_lMoney == 0)
	{
		return false;
	}
	this->setCarryMoney();

	if(coincfg.maxmoney>0 && this->m_lMoney>=coincfg.minmoney&& this->m_lMoney<=coincfg.maxmoney)
	{
		return true;
	}
	else if(coincfg.maxmoney<=0 && this->m_lMoney>=coincfg.minmoney)
	{
		return true;
	}
	else
		return false;
}

void Player::setCarryMoney()
{
	if(this->m_lMoney < coincfg.carrycoin)
		this->m_lCarryMoney = this->m_lMoney;
	else
		this->m_lCarryMoney = coincfg.carrycoin;

	if(Configure::getInstance()->level == PRIVATE_ROOM)
		this->m_lCarryMoney = this->m_lMoney;
}

int Player::setCarryInNextRound(int64_t money)
{
	//本身的金币大于本场次的最大带入金币
	if(this->m_lCarryMoney > coincfg.carrycoinmax)
		return -1;
	//身上金币都不够带入
	if(this->m_lMoney - this->m_lCarryMoney - this->betCoinList[0] < money)
		return -2;
	//想要带入金币大于最大带入金币
	if(this->m_lCarryMoney + money > coincfg.carrycoinmax)
		return -3;
	if(this->isActive())
		m_lAddMoney = money;
	else
		this->m_lCarryMoney += money;
	return 0;
}

void Player::calcMaxWin()
{
	if(m_lFinallGetCoin > this->m_lMaxWinMoney)
		m_lMaxWinMoney = m_lFinallGetCoin;
}


