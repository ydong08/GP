
#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "Logger.h"
#include "IProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "ProcessFactory.h"

//static int betnum[7]={100,1000,10000,100000,500000,1000000,5000000};
static int betnum[5] = {100,500,1000,5000,10000};
#define MAX_BET_NUM  (5)

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
	level  = 1;
	id = -1;         //用户id
	memset(name,0,sizeof(name));
	tid = 0;		//当前属于哪个桌子id
	tab_index = -1;//当前在桌子的位置
	money = 0;       //金币数
	nwin = 0;       //赢牌数
	nlose = 0;     //输牌数
	memset(json, 0, sizeof(json));        //json字符串
	status = 0;      //状态 0 未登录 1 在玩牌  
	source = 0; //用户来源
	clevel = 0;       //当前金币场底注 0-无底注
	isdropline = false; //是否已经掉线
	betNum = 0;
	bettype = 0;
	betcoin = 0;
	playNum = 0;
	willPlayCount = 0;
	bankeruid = 0;
	bankerwinCount = 0;
	bankerhasNum = 0;
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

int Player::heartbeat()
{
	IProcess* process = ProcessFactory::getProcesser(USER_HEART_BEAT);
    return process->doRequest(this->handler, NULL, NULL);
}

int getCoin(int64_t money)
{
	int randnum = rand()%MAX_BET_NUM;
	int willbetcoin = betnum[randnum];
	while(randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if(money/10 < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int getBetCoin(int64_t money)
{
	//return 100;
	int randnum = (rand()% (MAX_BET_NUM - 1));
	if(randnum < 2)
		randnum = 2;
	int willbetcoin = betnum[randnum];
	while(randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if(money/10 < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int getLowCoin(int64_t money)
{
	int randnum = rand()%2;
	int willbetcoin = betnum[randnum];
	while(randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if(money/10 < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int Player::betCoin()
{
	if(Configure::getInstance()->m_nLevel == 2)
	{
		if(this->betNum < 5)
		{
			this->betNum++;
			/*if(this->betcoin == 0)
				this->betcoin = getBetCoin(this->money);
			else
				this->betcoin = this->betcoin;
			if(this->betNum > 2)
				this->betcoin = getLowCoin(this->money);
			if(this->bettype == 0)
			{
				int num = rand()%2;
				this->bettype = (num == 0) ? 1 : 3;
			}
			else if(this->bettype == 1)
				this->bettype = 3;
			else
			{
				this->bettype = 1;
			}
			//this->bettype = ((num==1) ? 1 : 3);
			if(this->betcoin == 0)
				return 0;*/

			int num = rand()%100;
			if(num < 80)
			{
				this->betcoin = getBetCoin(this->money);
				int betnum = rand()%4;
				this->bettype = betnum + 1;
			}
			if(num >= 80 && num < 94)
			{
				this->betcoin = getLowCoin(this->money);
				int betnum = rand()%4;
				this->bettype = betnum + 1;
			}

		//	_LOG_DEBUG_("betnum:%d id:%d bettype:%d betcoin:%d\n", this->betNum, this->id, this->bettype, this->betcoin);
			if(this->betcoin == 0)
				return 0;
		}
		//第四次下注下莊或者闲天王
		/*else if (this->betNum == 3)
		{
			this->betNum++;	
			this->betcoin = getLowCoin(this->money);
			int num = rand()%2;
			this->bettype = (num == 0) ? 4 : 5;
			if(this->betcoin == 0)
				return 0;
		}
		else
			return 0;*/
	}

	if(Configure::getInstance()->m_nLevel == 3)
	{
		this->betNum++;
		if(this->bettype == 0)
		{
			int num = rand()%2;
			this->bettype = (num == 0) ? 1 : 3;
		}
		this->betcoin = getCoin(this->money);
		if(this->betcoin == 0)
			return 0;
	}
	IProcess* process = ProcessFactory::getProcesser(CLIENT_MSG_BET_COIN);
    return process->doRequest(this->handler, NULL, NULL);
}

void Player::startHeartTimer(int timeout)
{
	timer.startHeartTimer(timeout);
}

void Player::startBetTimer(int timeout)
{
	timer.startBetCoinTimer(timeout);
	printf("id:%d time:%d start time\n", this->id, timeout);
}

void Player::stopBetTimer()
{
	timer.stopBetCoinTimer();
}


