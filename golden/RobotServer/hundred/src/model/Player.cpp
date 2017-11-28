#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "Logger.h"
#include "BaseProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "Configure.h"

static int betnum[5]={ 100,500,1000,5000,10000 };


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
	BaseProcess* process = ProcessManager::getInstance().getProcesser(HALL_USER_LOGIN);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::logout()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_LEAVE);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::heartbeat()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(USER_HEART_BEAT);
    return process->doRequest(this->handler, NULL, NULL);
}

int getCoin(int64_t money)
{
	int randnum = rand()%5;
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
	int randnum = rand()%5;
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
	int randnum = rand()%3;
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
	if(Configure::getInstance().m_nLevel == 2)
	{
		if(this->betNum < 5)
		{
			this->betNum++;
			int num = rand()%100;
			this->betcoin = getBetCoin(this->money);
			if(num < 80)
			{
				int betnum = rand()%4;
				this->bettype = betnum + 1;
			}
			else
			{
				this->betcoin = getLowCoin(this->money);
				int betnum = rand()%4;
				this->bettype = betnum + 1;
			}
			LOGGER(E_LOG_DEBUG)<<"betnum:"<<this->betNum<<" id:"<<this->id<<" bettype:"<<this->bettype<<" betcoin:"<<this->betcoin;
			if(this->betcoin == 0)
				return 0;
		}
	}

	if(Configure::getInstance().m_nLevel == 3)
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
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_BET_COIN);
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


