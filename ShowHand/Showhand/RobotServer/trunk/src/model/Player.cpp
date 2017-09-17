#include "Player.h"
#include "Logger.h"
#include "IProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "ProcessFactory.h"

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
	carrycoin = 0;								//这盘牌携带金币数
	nwin = 0;									//赢牌数
	nlose = 0;									//输牌数
	memset(json, 0, sizeof(json));				//json字符串
	status = 0;									//状态 0 未登录 1 已登录  2 入座等待开局  3 正在游戏 4 结束游戏
	source = 0;									//用户来源
	clevel = 0;									//当前金币场底注 0-无底注
	memset(card_array, 0, sizeof(card_array));	//当前手牌
	memset(Knowcard_array, 0, sizeof(Knowcard_array));
	hascard = false;
	memset(player_array, 0, sizeof(player_array));

	memset(betCoinList, 0, sizeof(betCoinList));
	finalgetCoin = 0;
	finalcardvalue = -1;
	thisroundhasbet = false;
	hasallin = false;
	optype = 0;
	limitcoin = -1;
	isdropline = false;
	currcardvalue = 0;
	rasecoin = 0;
	bactive = false;
	playNum = 0;
	isKnow = false;
}

void Player::reset()
{
	memset(card_array, 0, sizeof(card_array));	//当前手牌
	memset(Knowcard_array, 0, sizeof(Knowcard_array));
	hascard = false;
	//memset(player_array, 0, sizeof(player_array));
	memset(betCoinList, 0, sizeof(betCoinList));
	rasecoin = 0;
	isKnow = false;
}

int Player::login()
{
	IProcess* process = ProcessFactory::getProcesser(HALL_USER_LOGIN);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::logout()
{
	IProcess* process = ProcessFactory::getProcesser(CLIENT_MSG_LEAVE);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::check()
{
	IProcess* process = ProcessFactory::getProcesser(CLIENT_MSG_LOOK_CARD);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::call()
{
	IProcess* process = ProcessFactory::getProcesser(CLIENT_MSG_BET_CALL);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::rase()
{
	IProcess* process = ProcessFactory::getProcesser(CLIENT_MSG_BET_RASE);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::allin()
{
	IProcess* process = ProcessFactory::getProcesser(CLIENT_MSG_ALL_IN);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::throwcard()
{
	IProcess* process = ProcessFactory::getProcesser(CLIENT_MSG_THROW_CARD);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::heartbeat()
{
	IProcess* process = ProcessFactory::getProcesser(USER_HEART_BEAT);
    return process->doRequest(this->handler, NULL, NULL);
}

bool Player::robotIsLargest()
{
	BYTE bRobotCardArray[5];
	BYTE bPlayerCardArray[5];
	memcpy(bRobotCardArray,this->card_array,sizeof(this->card_array));
	m_GameLogic.SortCard(bRobotCardArray,currRound==5 ? 5 : currRound + 1);
	this->currcardvalue = m_GameLogic.GetCardKind(bRobotCardArray,currRound==5 ? 5 : currRound + 1);
	printf("========================robot currcardvalue:%d\n", this->currcardvalue);
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(this->player_array[i].id != 0 && this->player_array[i].hascard)
		{
			memcpy(bPlayerCardArray,this->player_array[i].card_array + 1,currRound==5 ? 4 : currRound);
			if(m_GameLogic.CompareCard(bPlayerCardArray,currRound==5 ? 4 : currRound, bRobotCardArray, currRound==5 ? 5 : currRound + 1))
				return false;
		}
	}
	return true;
} 


bool Player::robotKonwIsLargest()
{
	BYTE bRobotCardArray[5];
	BYTE bPlayerCardArray[5];
	memcpy(bRobotCardArray,this->Knowcard_array,sizeof(this->Knowcard_array));
	m_GameLogic.SortCard(bRobotCardArray,5);
	this->currcardvalue = m_GameLogic.GetCardKind(bRobotCardArray,5);
	_LOG_DEBUG_("========================robot currcardvalue:%d\n", this->currcardvalue);
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(this->player_array[i].id != 0 && this->player_array[i].hascard)
		{
			memcpy(bPlayerCardArray,this->player_array[i].Knowcard_array,5);
			if(m_GameLogic.CompareCard(bPlayerCardArray,5, bRobotCardArray, 5))
				return false;
		}
	}
	return true;
} 

void Player::setRaseCoin()
{
	if(this->clevel == 1)
	{
		if(this->limitcoin - this->diffcoin <= this->ante*3)
			this->rasecoin = this->limitcoin - this->diffcoin;
		else
		{
			short mul = 0;
			int64_t score = this->limitcoin - this->diffcoin - this->ante*3; 
			if(score > 0)
			{
				int num = score/this->ante;
				if(num > 0)
					mul = rand()%num + 1;
			}
			if(this->currRound == 2)
			{
				if(mul > 20)
					mul = 20;
			}
			this->rasecoin =this->ante*3 + mul*this->ante;
		}
	}
	else if(this->clevel == 2)
	{
		if(this->limitcoin - this->diffcoin <= this->ante*3)
			this->rasecoin = this->limitcoin - this->diffcoin;
		else
		{
			short mul = 0;
			int64_t score = this->limitcoin - this->diffcoin - this->ante*3; 
			if(score > 0)
			{
				int num = score/this->ante;
				if(num > 0)
					mul = rand()%num + 1;
			}
			if(this->currRound == 2)
			{
				if(mul > 20)
					mul = 20;
			}
			this->rasecoin =this->ante*3 + mul*this->ante;
		}
	}
	else if(this->clevel == 3 || this->clevel == 4)
	{
		if(this->limitcoin - this->diffcoin <= this->ante*2)
			this->rasecoin = this->limitcoin - this->diffcoin;
		else
		{
			short mul = 0;
			int64_t score = this->limitcoin - this->diffcoin - this->ante*2; 
			if(score > 0)
			{
				int num = score/this->ante;
				if(num > 0)
					mul = rand()%num;
			}
			this->rasecoin =this->ante*2 + mul*this->ante;
		}
	}
}

void Player::setKnowRaseCoin()
{
	if(this->clevel == 1)
	{
		if(this->limitcoin - this->diffcoin <= this->ante*9)
			this->rasecoin = this->limitcoin - this->diffcoin;
		else
		{
			short mul = 0;
			int64_t score = this->limitcoin - this->diffcoin - this->ante*9; 
			if(score > 0)
			{
				int num = score/this->ante;
				if(num > 0)
					mul = rand()%num + 1;
			}
			this->rasecoin =this->ante*9 + mul*this->ante;
		}
	}
	else if(this->clevel == 2)
	{
		if(this->limitcoin - this->diffcoin <= this->ante*9)
			this->rasecoin = this->limitcoin - this->diffcoin;
		else
		{
			short mul = 0;
			int64_t score = this->limitcoin - this->diffcoin - this->ante*9; 
			if(score > 0)
			{
				int num = score/this->ante;
				if(num > 0)
					mul = rand()%num + 1;
			}
			this->rasecoin =this->ante*9 + mul*this->ante;
		}
	}
	else if(this->clevel == 3 || this->clevel == 4)
	{
		if(this->limitcoin - this->diffcoin <= this->ante*2)
			this->rasecoin = this->limitcoin - this->diffcoin;
		else
		{
			short mul = 0;
			int64_t score = this->limitcoin - this->diffcoin - this->ante*2; 
			if(score > 0)
			{
				int num = score/this->ante;
				if(num > 0)
					mul = rand()%num;
			}
			this->rasecoin =this->ante*2 + mul*this->ante;
		}
	}
}


//===========================Timer==================================
void Player::stopAllTimer()
{
	timer.stopAllTimer();
}

void Player::startBetCoinTimer(int uid,int timeout)
{
	timer.startBetCoinTimer(uid, timeout);
}

void Player::stopBetCoinTimer()
{
	timer.stopBetCoinTimer();
}

void Player::startLeaveTimer(int timeout)
{
	timer.startLeaveTimer(timeout);
}

void Player::stopLeaveTimer()
{
	timer.stopLeaveTimer();
}

void Player::startActiveLeaveTimer(int timeout)
{
	timer.startActiveLeaveTimer(timeout);
}

void Player::stopActiveLeaveTimer()
{
	timer.stopActiveLeaveTimer();
}

void Player::startHeartTimer(int uid,int timeout)
{
	timer.startHeartTimer(uid, timeout);
}

void Player::stopHeartTimer()
{
	timer.stopHeartTimer();
}




