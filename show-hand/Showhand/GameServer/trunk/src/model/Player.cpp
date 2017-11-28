#include <json/json.h>
#include "Player.h"
#include "AllocSvrConnect.h"
#include "Configure.h"
#include "Logger.h"
#include "CoinConf.h"
#include "IProcess.h"
#include "MoneyConnect.h"
#include "TaskRedis.h"

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
	tid = -1;									//牌座ID 
	tab_index = -1;								//当前在桌子的位置
	score = 0;									//积分
	money = 0;									//金币数
	carrycoin = 0;								//这盘牌携带金币数
	nwin = 0;									//赢牌数
	nlose = 0;									//输牌数
	nrunaway = 0;
	ntie = 0;
	memset(json, 0, sizeof(json));				//json字符串
	status = 0;									//状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
	source = 0;									//用户来源
	clevel = 0;									//当前金币场底注 0-无底注
	memset(card_array, 0, sizeof(card_array));	//当前手牌
	hascard = false;
	hallid = 0;									//属于哪个大厅ID

	active_time = 0;
	enter_time = 0;
	memset(betCoinList, 0, sizeof(betCoinList));
	finalgetCoin = 0;
	finalcardvalue = -1;
	thisroundhasbet = false;
	hasallin = false;
	isorder = false;
	memset(&coincfg,0,sizeof(CoinCfg));
	optype = 0;
	nextlimitcoin = -1;
	isdropline = false;
	istask = false;
	isroundtask = false;
	memset(m_byIP, 0, sizeof(m_byIP));

	cid = 0;
	sid = 0;
	pid = 0;
	timeoutCount = 0;
	startnum = 0;
	task1 = NULL;
	ningot = 0;
	voucher = 0;
	boardTask = -1;

	m_nRoundComFlag = 0;
	m_nRoundNum = 0;
	m_bFinalComFlag = 0;
	m_nComGetCoin = 0;
	m_nComGetRoll = 0;
	m_nMagicNum = 0;
	m_nMagicCoinCount = 0;
	isbacklist = false;

	deducte_tax = 0;
}

void Player::reset()
{
	memset(card_array, 0, sizeof(card_array));	//当前手牌
	hascard = false;
	memset(betCoinList, 0, sizeof(betCoinList));	
	finalgetCoin = 0;
	thisroundhasbet = false;
	finalcardvalue = -1;
	hasallin = false;
	optype = 0;
	isorder = false;
	nextlimitcoin = -1;
	task1 = NULL;
	ningot = 0;
	if(this->isGameOver())
	{
		_LOG_DEBUG_("player:%d is GameOver\n", this->id);
		m_nMagicCoinCount = 0;
		m_nMagicNum = 0;
	}
	isbacklist = false;
	deducte_tax = 0;
}

void Player::login()
{
	short clevel = Configure::getInstance()->level;
	/*if (Configure::getInstance()->level == 6 || Configure::getInstance()->level == 7)
	{
		clevel = 3;
	}*/
	coincfg.level = clevel;

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
			this->istask = (tasknum == 1) ? true : false;
			this->isroundtask = (tasknum == 10) ? true : false;
			if(this->isroundtask)
				this->istask = true;
			//是机器人则用传过来的战绩
			//if(source == 30)
			//{
				int totalNum = pase_json(value,"totalNum",0);
				int winNum = pase_json(value,"winNum",0);
				this->nwin = winNum;
				this->nlose = totalNum - winNum;
			//}
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

	if(this->source != 30 && this->isroundtask)
	{
		this->m_nRoundComFlag = TaskRedis::getInstance()->getCompleteFlag(this->id);
		if(m_nRoundComFlag < 0)
			this->m_nRoundComFlag = 0;
		this->m_nRoundNum = TaskRedis::getInstance()->getPlayCount(this->id);
		if(m_nRoundNum < 0)
			this->m_nRoundNum = 0;
		_LOG_DEBUG_("uid:%d m_nRoundComFlag:%d m_nRoundNum:%d\n", this->id, this->m_nRoundComFlag, this->m_nRoundNum);
	}
}

void Player::leave()
{
	AllocSvrConnect::getInstance()->userLeaveGame(this);
	MoneyNodes* pmconnect = MoneyConnectManager::getNodes(this->id % 10 + 1);
	OutputPacket respone;
	respone.Begin(UPDATE_LEAVE_GAME_SERVER);
	respone.WriteInt(this->id); 
	respone.End();
	if(this->source != 30)
	{
		if(pmconnect->send(&respone, false) < 0)
			_LOG_ERROR_("Send request to MoneyServer Error\n" );
	}
	IProcess::UpdateDBActiveTime(this);
	this->status = STATUS_PLAYER_LOGOUT;
	this->tid = -1;
	this->hascard = false;
	this->tab_index = -1;
}

void Player::enter()
{
	AllocSvrConnect::getInstance()->userUpdateStatus(this, this->status);
}

bool Player::checkMoney()
{

	CoinConf* coinCalc = CoinConf::getInstance();
	coinCalc->getCoinCfg(&coincfg);
	if (this->money == 0)
	{
		return false;
	}
	this->setCarryMoney();

	if(coincfg.maxmoney>0 && this->money>=coincfg.minmoney&& this->money<=coincfg.maxmoney)
	{
		return true;
	}
	else if(coincfg.maxmoney<=0 && this->money>=coincfg.minmoney)
	{
		return true;
	}
	else
		return false;
}

void Player::setCarryMoney()
{
	if(coincfg.level < 3)
	{
		if(this->money < coincfg.carrycoin)
			this->carrycoin = this->money;
		else
			this->carrycoin = coincfg.carrycoin;
	}
	//高级场和二人场做金币限制
	else if (coincfg.level == 3 || coincfg.level == 4)
	{
		if(this->money < coincfg.carrycoin + coincfg.retaincoin)
			this->carrycoin = this->money - coincfg.retaincoin;
		else
			this->carrycoin = coincfg.carrycoin;
	}
	else
		//总是让用户剩余1000金币
		this->carrycoin = this->money - coincfg.retaincoin;
}


