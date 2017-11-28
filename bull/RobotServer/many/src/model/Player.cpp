
#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "Logger.h"
#include "IProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "ProcessFactory.h"

static int betnum[5]={ 100,500,1000,5000,10000 };

int getCoin(int64_t money)
{
	int randnum = rand() % 5;
	int willbetcoin = betnum[randnum];
	while (randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if (money / 10 < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int getBetCoin(int64_t money)
{
	//return 100;
	int randnum = rand() % 3 + 2;
	int willbetcoin = betnum[randnum];
	while (randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if (money / 10 < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int getHighCoin(int64_t money)
{
	int randnum = rand() % 2 + 3;
	int willbetcoin = betnum[randnum];
	while (randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if (money / 10 < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int getLowCoin(int64_t money)
{
	int randnum = rand() % 3;
	int willbetcoin = betnum[randnum];
	while (randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if (money / 10 < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
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

int Player::betCoin()
{
	if(Configure::getInstance()->m_nLevel == 2)
	{
		for (int i = 0; i < rand() % 10 + 1; i++)
		{
			if (this->betNum < 10)
			{
				int bt = rand() % 100;
				if (bt < 25)
				{
					this->bettype = 1;
				}
				else if (bt >= 25 && bt < 50)
				{
					this->bettype = 2;
				}
				else if (bt >= 50 && bt < 75)
				{
					this->bettype = 3;
				}
				else
				{
					this->bettype = 4;
				}
				int num = rand() % 1000;
				if (num < 500)
				{
					this->betcoin = getCoin(this->money);
				}
				else if (num >= 500 && num < 750)
				{
					this->betcoin = getLowCoin(this->money);
				}
				else if (num >= 750 && num < 980)
				{
					this->betcoin = getBetCoin(this->money);
				}
				else
				{
					this->betcoin = 0;	//极低几率不下注
				}
				if (PlayerManager::getInstance()->CheckAreaBetLimit(this->bettype, this->betcoin))
				{
					this->betNum++;
					LOGGER(E_LOG_DEBUG) << "betnum:" << this->betNum << " id:" << this->id << " bettype:" << this->bettype << " betcoin:" << this->betcoin;
				}
				else
				{
					LOGGER(E_LOG_INFO) << "out of bet limit:" << PlayerManager::getInstance()->areaTotalBetLimit[bettype] << " bet coin = " << this->betcoin
						<< " bettype = " << this->bettype;
					return 0;
				}
				if (this->betcoin == 0)
				{
					LOGGER(E_LOG_INFO) << "betcoin = 0";
					return 0;
				}
			}
		}
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


