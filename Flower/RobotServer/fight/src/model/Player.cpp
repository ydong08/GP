#include "Player.h"
#include "Logger.h"
#include "BaseProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "Configure.h"
#include "RobotDefine.h"

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


void Player::init()
{
	timer.init(this);
	id = 0;
	memset(name, 0, sizeof(name));
	tid = -1;									//牌座ID 
	tab_index = -1;								//当前在桌子的位置
	score = 0;									//积分
	money = 0;									//金币数
	memset(json, 0, sizeof(json));				//json字符串
	memset(player_array, 0, sizeof(player_array));
	hascard = 0;
	isbanker = false;
	CardType = 0;
	playCount = 0;
	iscall = 0;
	mulindex = 0;
	bankeruid = 0;
	isKnow = true;
	rasecoin = 0;
	compareUid = 0;
	currRound = 0;
	sumbetcoin = 0;
	memset(rasecoinarray, 0, sizeof(rasecoinarray));
	countnum = 0;
	bcheck = false;

	m_nAwayCount = 0;
	m_bBetCoinTimer = false;
	m_bCheckTimer = false;
	m_betCoinTime = 0;
	m_checkTime = 0;
	m_vAwayUid.clear();
}

void Player::reset()
{
	memset(card_array, 0, sizeof(card_array));	//当前手牌
	memset(Knowcard_array, 0, sizeof(Knowcard_array));
	hascard = 0;
	isbanker = false;
	CardType = 0;
	iscall = 0;
	mulindex = 0;
	bankeruid = 0;
	isKnow = true;
	rasecoin = 0;
	compareUid = 0;
	currRound = 0;
	sumbetcoin = 0;
	countnum = 0;
	bcheck = false;
	
	m_nAwayCount = 0;
	m_bBetCoinTimer = false;
	m_bCheckTimer = false;
	m_betCoinTime = 0;
	m_checkTime = 0;
	m_vAwayUid.clear();
}

int Player::login()
{
	BaseProcess* process = GET_PROCESS(HALL_USER_LOGIN);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::logout()
{
	BaseProcess* process = GET_PROCESS(CLIENT_MSG_LEAVE);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::heartbeat()
{
	BaseProcess* process = GET_PROCESS(USER_HEART_BEAT);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::gamestart()
{
	BaseProcess* process = GET_PROCESS(CLIENT_MSG_START_GAME);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::call()
{
	BaseProcess* process = GET_PROCESS(CLIENT_MSG_BET_CALL);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::rase()
{
	BaseProcess* process = GET_PROCESS(CLIENT_MSG_BET_RASE);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::throwcard()
{
	BaseProcess* process = GET_PROCESS(CLIENT_MSG_THROW_CARD);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::allin()
{
	BaseProcess* process = GET_PROCESS(CLIENT_MSG_ALL_IN);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::comparecard()
{
	compareUid = 0;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(this->player_array[i].id != 0 && this->player_array[i].hascard > 0)
		{
			if(this->money < currmax*2 || this->player_array[i].forbidround == 0)
			{
				compareUid = this->player_array[i].id;
				break;
			}
		}
	}

	LOGGER(E_LOG_DEBUG) << "compareUid=" << compareUid;
	if(compareUid == 0)
	{
		if(this->robotKonwIsLargest())
			return call();
		int randnum = rand()%10;
		if(randnum < 6)
			return throwcard();
		return call();
	}
	
	BaseProcess* process = GET_PROCESS(CLIENT_MSG_COMPARE_CARD);
	if (process != NULL)
	{
		return process->doRequest(this->handler, NULL, NULL);
	}
	assert(0);
	return -1;
}

int Player::check()
{
	if(this->currRound >= LIMIT_ROBOT_CHECK_ROUND)
	{
		if (this->hascard == 1)
		{
			if (bcheck) {
				return 0;
			}
			bcheck = true;

			BaseProcess* process = GET_PROCESS(CLIENT_MSG_CHECK_CARD);
			if (process != NULL)
			{
				return process->doRequest(this->handler, NULL, NULL);
			}
			assert(0);
			return -1;
		}
	}
	return 0;
}

bool Player::isRaseMax()
{
	if(currmax == rasecoinarray[4])
		return true;
	return false;
}

bool Player::hasSomeCheck()
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		LOGGER(E_LOG_DEBUG) << "i:" << i << " id:" << player_array[i].id << " hascard:" << player_array[i].hascard;
		if(this->player_array[i].id != 0 && this->player_array[i].hascard == 2)
		{
			return true;
		}
	}
	return false;
}

bool Player::robotKonwIsLargest()
{
	BYTE bRobotCardArray[3];
	BYTE bPlayerCardArray[3];
	memcpy(bRobotCardArray,this->Knowcard_array,sizeof(this->Knowcard_array));
	m_GameLogic.SortCardList(bRobotCardArray, 3);
	BYTE mytype = m_GameLogic.GetCardType(bRobotCardArray,3);
	this->CardType = mytype;
	this->maxValue = bRobotCardArray[0];

	LOGGER(E_LOG_DEBUG) << "robot:" << this->id << " currcardvalue:" << mytype << " maxValue:" << this->maxValue;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(this->player_array[i].id != 0 && this->player_array[i].hascard > 0)
		{
			memcpy(bPlayerCardArray,this->player_array[i].Knowcard_array,3);
			m_GameLogic.SortCardList(bPlayerCardArray, 3);
			BYTE othertype = m_GameLogic.GetCardType(bPlayerCardArray,3);
			LOGGER(E_LOG_DEBUG) << "uid:" << this->player_array[i].id << " othertype:" << othertype;
			if(m_GameLogic.CompareCard(bPlayerCardArray, bRobotCardArray, 3) > 0)
				return false;
		}
	}
	return true;
} 

bool Player::isCanGameOver()
{
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(this->player_array[i].id != 0 && this->player_array[i].hascard > 0)
		{
			return false;
		}
	}
	return true;
}

void Player::setRaseCoin()
{
	short tempindex = 0;
	for(int i = 4; i >= 0; i--)
	{
		if(this->currmax >= this->rasecoinarray[i])
		{
			tempindex = i;
			break;
		}
	}
	short diff = 4 - tempindex;

	if(diff > 0)
		this->rasecoin = this->rasecoinarray[tempindex+(rand()%diff + 1)];
	else
		this->rasecoin = 0;
}

//===========================Timer==================================
void Player::stopAllTimer()
{
	timer.stopAllTimer();
}

void Player::startLeaveTimer(int timeout)
{
	timer.startLeaveTimer(timeout);
}

void Player::stopLeaveTimer()
{
	timer.stopLeaveTimer();
}

void Player::startHeartTimer(int uid,int timeout)
{
	timer.startHeartTimer(uid, timeout);
}

void Player::stopHeartTimer()
{
	timer.stopHeartTimer();
}

void Player::startGameStartTimer(int uid,int timeout)
{
	timer.startGameStartTimer(uid, timeout);
}

void Player::stopGameStartTimer()
{
	timer.stopGameStartTimer();
}

void Player::startBetCoinTimer(int uid,int timeout)
{
	m_bBetCoinTimer = true;
	m_betCoinTime = timeout;
	timer.startBetCoinTimer(uid, timeout);
}

void Player::stopBetCoinTimer()
{
	timer.stopBetCoinTimer();
}

void Player::startCheckTimer(int uid,int timeout)
{
	m_bCheckTimer = true;
	m_checkTime = timeout;
	timer.startCheckTimer(uid, timeout);
}

void Player::stopCheckTimer()
{
	timer.stopCheckTimer();
}