
#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "Logger.h"
#include "BaseProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "Configure.h"

//static int betnum[7]={100, 500, 1000, 2000, 5000, 10000, 50000};
static int betnum[6] = { 100, 500, 1000, 5000, 10000, 50000 };

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
    memset(m_lBetArray , 0 , sizeof(m_lBetArray));
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
	int randnum = rand()%6;
	int willbetcoin = betnum[randnum];
	while(randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if(money < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int getBetCoin(int64_t money)
{
	//return 100;
	int randnum = rand()%6;
	if(randnum < 2)
		randnum = 2;
	int willbetcoin = betnum[randnum];
	while(randnum >= 0)
	{
		int willbetcoin = betnum[randnum];
		if(money < willbetcoin)
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
		if(money < willbetcoin)
			--randnum;
		else
			return willbetcoin;
	}
	return 0;
}

int Player::betCoin()
{
    srand(time(NULL));

	if(Configure::getInstance()->level == 3)
	{
		for (int i = 0; i < rand() % 10 + 1; i++)
		{
			if (this->betNum < 10)
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

					// 			int num = rand()%1000;
					// 			this->betcoin = getBetCoin(this->money);
					// 			if(num < 750)
					// 			{
					// 				int betnum = rand()%2;
					// 				this->bettype = ((betnum==1) ? 4 : 5);
					// 			}
					// 			if(num >= 750 && num < 850)
					// 			{
					// 				this->betcoin = getLowCoin(this->money);
					// 				int betnum = rand()%2;
					// 				this->bettype = ((betnum==1) ? 1 : 3);
					// 			}
					// 
					// 			if(num >= 850 && num < 1000)
					// 			{
					// 				int randmin = rand()%100;
					// 				if(randmin < 60)
					// 					this->betcoin = getLowCoin(this->money);
					// 
					// 				this->bettype = 2;
					// 			}

                int bt = rand() % 100;
                int num = rand() % 1000;
                int big_rate = rand() % 100;

               _LOG_DEBUG_("当前下注情况: 总压闲[%ld], 总压庄[%ld], 总压和[%ld], 总压庄对[%ld],  总压闲对[%ld]" ,
                    m_lBetArray[4] , m_lBetArray[5] , m_lBetArray[2] , m_lBetArray[3] , m_lBetArray[1]);
                //庄闲下注区域总额偏差超过 70%，选择小额区域下注
                if (m_lBetArray[4] > m_lBetArray[5] && m_lBetArray[4] - m_lBetArray[5] > 500 && m_lBetArray[5] * 100 / m_lBetArray[4] <= 70)
                {
                    if (big_rate < 50)
                    {
                        this->betcoin = getBetCoin(this->money);
                    }
                    else if (big_rate < 90)
                    {
                        this->betcoin = 0;
                    }
                    else
                    {
                        this->betcoin = getLowCoin(this->money);
                    }
                    _LOG_DEBUG_("总压闲太少，机器人:%s 压闲[%d]", this->name, this->betcoin);

                    this->bettype = 5;        // 压闲
                }
                else if (m_lBetArray[5] > m_lBetArray[4] && m_lBetArray[5] - m_lBetArray[4] > 500 && m_lBetArray[4] * 100 / m_lBetArray[5] <= 70)
                {
                    if (big_rate < 50)
                    {
                        this->betcoin = getBetCoin(this->money);
                    }
                    else if (big_rate < 90)
                    {
                        this->betcoin = 0;
                    }
                    else
                    {
                        this->betcoin = getLowCoin(this->money);
                    }
                    _LOG_DEBUG_("总压庄太少，机器人:%s 压庄", this->name, this->betcoin);

                    this->bettype = 4;        // 压庄
                }
                else
                {
                    //if (bt < 75)
                    if (bt < 40) // 40%概率压庄或闲
                    {
                        this->bettype = 4;
                        //if (num < 500)
                        if (num < 300)
                        {
                            if (m_lBetArray[5] - m_lBetArray[4] > 100)
                            {
                                this->betcoin = getBetCoin(this->money);
                            }
                            else
                            {
                                this->betcoin = getLowCoin(this->money);
                            }
                        }
                        //else if (num >= 500 && num < 980)
                        else if (num >= 300 && num < 600)
                        {
                            this->bettype = 5;
                            if (m_lBetArray[5] - m_lBetArray[4] > 100)
                            {
                                this->betcoin = getBetCoin(this->money);
                            }
                            else
                            {
                                this->betcoin = getLowCoin(this->money);
                            }
                        }
                        else
                        {
                            this->betcoin = 0;	//极低几率不下注
                        }
                    }
                    //else if (bt >= 75 && bt < 85)  
                    else if (bt >= 40 && bt < 50)  
                    {
                        this->bettype = 1;
                        if (num < 200)  // 5%压闲对
                        {
                            this->betcoin = getLowCoin(this->money);
                        }
                        else if (num >= 200 && num < 500)  // // 不到5%的概率压庄对
                        {
                            this->bettype = 3;
                            this->betcoin = getLowCoin(this->money);
                        }
                        else
                        {
                            this->betcoin = 0;	//极低几率不下注
                        }
                    }
                    else
                    {
                        this->bettype = 2;
                        // 5%的概率压庄对， 大额压的概率是2.5%， 小额压的概率是2.5%
                        //if (num < 980)
                        if (num < 26)
                        {
                            this->betcoin = getLowCoin(this->money);
                        }
                        //else if (num >= 980 && num < 995)
                        else if (num >= 26 && num < 29)
                        {
                            this->betcoin = getBetCoin(this->money);
                        }
                        else
                        {
                            this->betcoin = 0;	//极低几率不下注
                        }
                    }
                }

				// 			if (num < 500)
				// 			{
				// 				this->betcoin = getCoin(this->money);
				// 			}
				// 			else if (num >= 500 && num < 750)
				// 			{
				// 				this->betcoin = getLowCoin(this->money);
				// 			}
				// 			else if (num >= 750 && num < 980)
				// 			{
				// 				this->betcoin = getBetCoin(this->money);
				// 			}
				// 			else
				// 			{
				// 				this->betcoin = 0;	//极低几率不下注
				// 			}

				_LOG_DEBUG_("id:%d bettype:%d betcoin:%d\n", this->id, this->bettype, this->betcoin);
				if (this->betcoin == 0)
					continue;
				BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_BET_COIN);
				return process->doRequest(this->handler, NULL, NULL);
			}
		}
	}

// 	if(Configure::getInstance()->level == 1)
// 	{
// 		this->betNum++;
// 		if(this->bettype == 0)
// 		{
// 			int rate = rand() % 1000;
// 			if (rate <= 850)
// 			{
// 				int num = rand() % 2;
// 				this->bettype = (num == 0) ? 1 : 3;
// 			}
// 			else
// 			{
// 				this->bettype = 2;
// 			}
// 		}
// 		this->betcoin = getCoin(this->money);
// 		if(this->betcoin == 0)
// 			return 0;
// 	}
	//this->betcoin /= 1000;
	return 0;
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


