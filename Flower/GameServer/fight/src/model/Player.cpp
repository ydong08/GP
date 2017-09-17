#include <json/json.h>
#include "Player.h"
#include "AllocSvrConnect.h"
#include "Configure.h"
#include "Logger.h"
#include "CoinConf.h"
#include "IProcess.h"
#include "ProtocolServerId.h"
#include "GameCmd.h"
#include "MoneyAgent.h"
#include "GameApp.h"
#include "Logger.h"

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
	m_nWin = 0;										//赢牌数
	m_nLose = 0;									//输牌数
	m_nRunAway = 0;
	m_nTie = 0;
	memset(json, 0, sizeof(json));					//json字符串
	m_nStatus = 0;									//状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
	source = 0;										//用户来源
	m_nType = 0;									//当前金币场底注 0-无底注
	memset(card_array, 0, sizeof(card_array));		//当前手牌
	m_bCardStatus = CARD_DISCARD;
	m_nHallid = 0;									//属于哪个大厅ID
	memset(m_byIP, 0, sizeof(m_byIP));

	active_time = 0;
	enter_time = 0;
	m_lFinallGetCoin = 0;
	m_nEndCardType = 0;
	optype = 0;
	isdropline = false;
	cid = 0;
	sid = 0;
	pid = 0;
	timeoutCount = 0;
	startnum = 0;
	m_nRoll = 0;
	m_bCompleteTask = false;
	m_nGetRoll = 0;
	m_nMagicNum = 0;
	m_nMagicCoinCount = 0;

	m_lMaxWinMoney = 0;
	m_lMaxCardValue = 0;
	m_lSumBetCoin = 0;
	m_bAllin = false;
	m_setCompare.clear();
	m_AllinCoin = 0;
	m_bCompare = false;
	m_nUseMulNum = 0;
	m_nUseForbidNum = 0;
	m_nChangeNum = 0;
	m_nUseMulCoin = 0;
	m_nUseForbidCoin = 0;
	m_nUseChangeCoin = 0;
	isbacklist = false;
    is_discard = false;
	m_nTax = 0;
}

void Player::reset()
{	
	memset(card_array, 0, sizeof(card_array));	//当前手牌
	m_bCardStatus = CARD_DISCARD;
	m_lFinallGetCoin = 0;
	m_nEndCardType = 0;
	optype = 0;
	m_bCompleteTask = false;
	m_nGetRoll = 0;
	m_nMagicNum = 0;
	m_nMagicCoinCount = 0;
	m_lSumBetCoin = 0;
	m_bAllin = false;
	m_setCompare.clear();
	m_AllinCoin = 0;
	m_bCompare = false;
	m_nUseMulNum = 0;
	m_nUseForbidNum = 0;
	m_nChangeNum = 0;
	m_nUseMulCoin = 0;
	m_nUseForbidCoin = 0;
	m_nUseChangeCoin = 0;
	isbacklist = false;
    is_discard = false;
	m_nTax = 0;
}

void Player::login()
{
	short m_nType = Configure::getInstance()->m_nLevel;
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
			int loseNum = pase_json(value,"loseTimes",20);
			int winNum = pase_json(value,"winTimes",5);
			double maxwin = pase_json(value, "maxwin", 0.0);
			double maxvalue = pase_json(value, "maxvalue", 0.0);
			this->m_nWin = winNum;
			this->m_nLose = loseNum;
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
}

void Player::leave()
{
	AllocSvrConnect::getInstance()->userLeaveGame(this);
    OutputPacket respone;
	respone.Begin(CMD_UPDATE_LEAVE_GAME_SERVER);
	respone.WriteInt(this->id); 
	respone.End();
	if(this->source != 30)
	{
		if (MoneyServer()->Send(&respone) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );
	}
	IProcess::UpdateDBActiveTime(this);
	this->m_nStatus = STATUS_PLAYER_LOGOUT;
	this->tid = -1;
	this->m_bCardStatus = CARD_DISCARD;
	this->m_nTabIndex = -1;
}

void Player::enter()
{
	AllocSvrConnect::getInstance()->userUpdateStatus(this, this->m_nStatus);
}

bool Player::checkMoney()
{
	CoinCfg* pCoincfg = CoinConf::getInstance()->getCoinCfg();

	if (this->m_lMoney == 0)
	{
		return false;
	}

	if(pCoincfg->maxmoney>0 && this->m_lMoney>= pCoincfg->minmoney&& this->m_lMoney<= pCoincfg->maxmoney)
	{
		return true;
	}
	else if(pCoincfg->maxmoney<=0 && this->m_lMoney>= pCoincfg->minmoney)
	{
		return true;
	}
	else
		return false;
}

bool Player::hasUseCoin()
{
	if(this->m_nUseMulCoin > 0 || this->m_nUseForbidCoin > 0 || 
		this->m_nUseChangeCoin > 0 || this->m_nMagicCoinCount > 0)
		return true;
	return false;
}

void Player::calcMaxWin()
{
	if(m_lFinallGetCoin > this->m_lMaxWinMoney)
			m_lMaxWinMoney = m_lFinallGetCoin;
}

int64_t Player::calculateCompMoney(Player * player, BYTE betmaxcard, int64_t betmax, int maxlimit)
{
	int64_t comparecoin = 0;
	if (player->m_bCardStatus == CARD_UNKNOWN)
	{
		return betmax;
	}

	if (20 == betmax)
		return 50;
	else
		return (betmax << 1);
// 
// 	if (player->m_bCardStatus == betmaxcard)
// 	{
// 		comparecoin = betmax;
// 		LOGGER(E_LOG_INFO) << "same card status:" << "player id = " << player->id << " card status = " << betmaxcard;
// 	}
// 	else
// 	{
// 		if (CARD_UNKNOWN == betmaxcard)
// 		{
// 			LOGGER(E_LOG_INFO) << "known vs unknow:" << "player id = " << player->id;
// 
// 			if (betmax == (maxlimit << 1))
// 			{
// 				comparecoin = betmax;
// 			}
// 			else
// 			{
// 				if (20 == betmax)
// 					comparecoin = 50;
// 				else
// 					comparecoin = (betmax << 1);
// 			}
// 		}
// 		else
// 		{
// 			LOGGER(E_LOG_INFO) << "unknow vs known:" << "player id = " << player->id;
// 
// 			if (50 == betmax)
// 				comparecoin = 20;
// 			else
// 				comparecoin = (betmax>>1);
// 		}
// 	}
// 
// 	return comparecoin;
}


void Player::recordCard(std::string & cards)
{
    game_play_record["cards"] = cards;
}

void Player::recordThrowCard(int round , int64_t sumBetCoin)
{
    char roundKey[64] = { 0 };
    sprintf(roundKey , "%d" , round);
    if (game_play_record.isMember(roundKey)) //本轮已经有记录
    {
        Json::Value& data = game_play_record[roundKey];
        data["throw"] = true;
    }
    else
    {
        Json::Value data;
        data["throw"] = true;
        game_play_record[roundKey] = data;
    }
}

void Player::recordCompareCard(int round , int otherUid , int64_t compareMoney, int result)
{
    char roundKey[64] = { 0 };
    sprintf(roundKey , "%d" , round);
    if (game_play_record.isMember(roundKey)) //本轮已经有记录
    {
        Json::Value& data = game_play_record[roundKey];
        data["otherUid"] = otherUid;
        data["compareMoney"] = (double)compareMoney;
        data["result"] = result;
    }
    else
    {
        Json::Value data;
        data["otherUid"] = otherUid;
        data["compareMoney"] = (double)compareMoney;
        data["result"] = result;
        game_play_record[roundKey] = data;
    }
}

void Player::recordActiveLeave(int round)
{
    char roundKey[64] = { 0 };
    sprintf(roundKey , "%d" , round);
    if (game_play_record.isMember(roundKey)) //本轮已经有记录
    {
        Json::Value& data = game_play_record[roundKey];
        data["leave"] = true;
    }
    else
    {
        Json::Value data;
        data["leave"] = true;
        game_play_record[roundKey] = data;
    }
}

void Player::recordRaise(int round , int64_t raiseMoney)
{
    char roundKey[64] = { 0 };
    sprintf(roundKey , "%d" , round);
    if (game_play_record.isMember(roundKey)) //本轮已经有记录
    {
        Json::Value& data = game_play_record[roundKey];
        data["raise"] = (double)raiseMoney;
    }
    else
    {
        Json::Value data;
        data["raise"] = (double)raiseMoney;
        game_play_record[roundKey] = data;
    }
}

void Player::recordCall(int round , int64_t callMoney)
{
    char roundKey[64] = { 0 };
    sprintf(roundKey , "%d" , round);
    if (game_play_record.isMember(roundKey)) //本轮已经有记录
    {
        Json::Value& data = game_play_record[roundKey];
        data["call"] = (double)callMoney;
    }
    else
    {
        Json::Value data;
        data["call"] = (double)callMoney;
        game_play_record[roundKey] = data;
    }
}

void Player::recordCheck(int round , int64_t checkMoney)
{
    char roundKey[64] = { 0 };
    sprintf(roundKey , "%d" , round);
    if (game_play_record.isMember(roundKey)) //本轮已经有记录
    {
        Json::Value& data = game_play_record[roundKey];
        data["check"] = (double)checkMoney;
    }
    else
    {
        Json::Value data;
        data["check"] = (double)checkMoney;
        game_play_record[roundKey] = data;
    }
}

void Player::recordAllIn(int round , int64_t allInMoney)
{
    char roundKey[64] = { 0 };
    sprintf(roundKey , "%d" , round);
    if (game_play_record.isMember(roundKey)) //本轮已经有记录
    {
        Json::Value& data = game_play_record[roundKey];
        data["allin"] = (double)allInMoney;
    }
    else
    {
        Json::Value data;
        data["allin"] = (double)allInMoney;
        game_play_record[roundKey] = data;
    }
}

void Player::recordWin(int64_t winMoney)
{
    game_play_record["win"] = (double)winMoney;
}

Json::Value& Player::gameRecordAnalysisInfo()
{
    return game_play_record;
}

void Player::setFinalGetCoin(int64_t delta_coin)
{
	ULOGGER(E_LOG_INFO, id) << "final getted coin changed, before = " << m_lFinallGetCoin << ", delta_coin = " << delta_coin;
	m_lFinallGetCoin += delta_coin;
	ULOGGER(E_LOG_INFO, id) << "final getted coin changed, after = " << m_lFinallGetCoin;
}
