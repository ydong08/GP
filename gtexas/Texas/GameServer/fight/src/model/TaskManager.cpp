#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "TaskManager.h"
#include "Logger.h"
#include "hiredis.h"
#include "Configure.h"
#include "Table.h"
#include "Util.h"

using namespace std;

static TaskManager * Instance = NULL;

static vector<string> split(const string& src,const string& tok, bool trim , string null_subst );


TaskManager::TaskManager()
{
	countTask = 0;
	index = 0;
	memset(taskarry, 0, sizeof(taskarry));
	memset(taskEsay, 0, sizeof(taskEsay));
	countEasy = 0;
}

TaskManager * TaskManager::getInstance()
{
	if(Instance==NULL)
		Instance  = new TaskManager();
	
	return Instance;
}

int TaskManager::init(short loadtype)
{
	if(loadTask(loadtype)!=0)
		return -1;
	return 0;
}

static redisContext * initRedisContext(const char * addr)
{
	struct timeval timeout = { 1, 0 };
	AddressInfo addrinfo;
	Util::parseAddress(addr, addrinfo);
	redisContext* redis = redisConnectWithTimeout(addrinfo.ip.c_str(), addrinfo.port, timeout);

	if ((NULL == redis) || (redis->err))   //连接redis
	{
		LOGGER(E_LOG_INFO) << "connect redis failed, ip = " << addrinfo.ip << " port = " << addrinfo.port << " err = " << redis->errstr;
		if (redis)
		{
			redisFree(redis);
			redis = NULL;
		}
		return NULL;
	}

	if (REDIS_OK != redisSetTimeout(redis, timeout))
	{
		LOGGER(E_LOG_INFO) << "redis set timeout failed...";
		redisFree(redis);
		redis = NULL;
		return NULL;
	}
	return redis;
}


int TaskManager::loadTask(short loadtype)
{
	redisContext *m_redis = NULL;  
    redisReply *reply = NULL;

    m_redis = initRedisContext(Configure::getInstance()->m_sServerRedisAddr[WRITE_REDIS_CONF].c_str());
	if (NULL == m_redis)
		return -1;

	_LOG_INFO_("Initredis-connect OK...\n");

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGETALL texastask:%d",loadtype));
    if(reply == NULL)
    {
         _LOG_ERROR_("Redis can't get texastask:%d\n",loadtype);
		 redisFree(m_redis);
		 return -1;
    }
	else
	{
		if (reply->type == REDIS_REPLY_ARRAY) 
		{
			if(reply->elements==0){
				_LOG_ERROR_("Can't get texastask:%d,task maybe not add\n",loadtype);
				freeReplyObject(reply);
				redisFree(m_redis);
				return -1; 
			}
			for (unsigned short j = 0; j < reply->elements; j++) 
			{
				if(reply->element[j]->type==REDIS_REPLY_NIL){
					freeReplyObject(reply);
					redisFree(m_redis);
					return -1; 
				}
				//printf("%s\n",reply->element[j]->str);
				if(!(j & 1))
				{
					taskarry[countTask].taskid = atol(reply->element[j]->str);
					//printf("taskarry[countTask].taskid:%d countTask:%d\n", taskarry[countTask].taskid, countTask);
				}
				else
				{
					vector<std::string> vlist = split(std::string(reply->element[j]->str), "|", true, "");
					if(vlist.size() == 3)
					{
						//printf("vlist[0]:%s vlist[1]:%s\n", vlist[0].c_str(), vlist[1].c_str());
						strncpy(taskarry[countTask].taskname,vlist[0].c_str(), sizeof(taskarry[countTask].taskname));
						taskarry[countTask].taskname[sizeof(taskarry[countTask].taskname) - 1] = '\0';
						taskarry[countTask].ningotlow = atoi(vlist[1].c_str());
						taskarry[countTask].ningothigh = atoi(vlist[2].c_str());
						//简单的任务列表
						if(taskarry[countTask].taskid == 1 || taskarry[countTask].taskid == 1048578 || taskarry[countTask].taskid == 2097154
							|| taskarry[countTask].taskid == 3145730 || taskarry[countTask].taskid == 4194306 )
						{
							taskEsay[countEasy++] = &taskarry[countTask];
						}
						++countTask;
					}
				}
			}
			freeReplyObject(reply);
		}
		else
		{
			freeReplyObject(reply);
			redisFree(m_redis);
			return -1;
		}
	}

	redisFree(m_redis);
	for(int i = 0; i < countTask; ++i)
	{
		printf("%ld:%s|%d----%d\n", taskarry[i].taskid, taskarry[i].taskname, taskarry[i].ningotlow, taskarry[i].ningothigh);
	}
    return 0;
}

Task * TaskManager::getTask()
{
	if(countTask == 0)
		return NULL;
	if(index >= countTask)
		index = 0;
	return &taskarry[index++];
}

Task * TaskManager::getRandTask()
{
	Task* ptask = NULL;
	if(countTask == 0)
		return NULL;
	int index = rand()%countTask;
	ptask = &taskarry[index];
	int easytask = rand()%100;
	//_LOG_DEBUG_("index:%d easytask:%d taskid:%d name:%s\n", index, easytask, ptask->taskid, ptask->taskIOSname);
	if(easytask < Configure::getInstance()->esayTaskRand)
		return ptask;
	else
	{
		if(isEsayTask(ptask->taskid))
		{
			while(1)
			{
				//_LOG_DEBUG_("taskid:%d name:%s\n", ptask->taskid, ptask->taskIOSname);
				index = index + 1 + rand()%9;
				index =index%countTask;
				ptask = &taskarry[index];
				if(!isEsayTask(ptask->taskid))
					return ptask;
			}
		}
		else
			return ptask;
	}
}

bool TaskManager::isEsayTask(int64_t taskid)
{
	for(int i = 0; i < countEasy; i++)
	{
		if(taskEsay[i]->taskid == taskid)
			return true;
	}
	return false;
}

Task * TaskManager::getRandEsayTask()
{
	if(countEasy == 0)
		return NULL;
	return taskEsay[rand()%countEasy];
}

bool TaskManager::calcWinOrAllin(Player* player, Table* table, short winflag)
{
	if(winflag != 0)
	{
		if(winflag == 1)
			return true;
		if(winflag == 2 && player->m_bAllin)
			return true;
		return false;
	}
	return false;
}

bool TaskManager::calcCardType(Player* player, Table* table, short cardtype)
{
	if(cardtype != 0)
	{
		if(table->m_bCurrRound < 5)
		{
			if(cardtype == player->m_bTempCardType)
				return true;
		}
		else
		{
			if(cardtype == player->m_nEndCardType)
				return true;
		}
		return false;
	}
	return false;
}

bool TaskManager::calcCardValue(Player* player, Table* table, short cardnum, short cardvalue)
{
	if(cardnum != 0 && cardvalue != 0)
	{
		short tempnum = 0;
		for(int i = 0; i < 2; ++i)
		{
			if(table->m_GameLogic.GetCardValue(player->card_array[i]) == cardvalue)
				tempnum++;
		}

		if(table->m_bCurrRound == 2)
		{
			for(int i = 0; i < 3; ++i)
			{
				if(table->m_GameLogic.GetCardValue(table->m_cbCenterCardData[i]) == cardvalue)
					tempnum++;
			}
		}

		if(table->m_bCurrRound == 3)
		{
			for(int i = 0; i < 4; ++i)
			{
				if(table->m_GameLogic.GetCardValue(table->m_cbCenterCardData[i]) == cardvalue)
					tempnum++;
			}
		}

		if(table->m_bCurrRound >= 4)
		{
			for(int i = 0; i < 5; ++i)
			{
				if(table->m_GameLogic.GetCardValue(table->m_cbCenterCardData[i]) == cardvalue)
					tempnum++;
			}
		}
		if(tempnum == cardnum)
			return true;
		return false;
	}
	return false;
}

bool TaskManager::calcBottomType(Player* player, Table* table, short bottomtype)
{
	if(bottomtype != 0)
	{
		if(bottomtype == 1)
		{
			if(table->m_GameLogic.GetCardValue(player->card_array[0]) == table->m_GameLogic.GetCardValue(player->card_array[1]))
				return true;
		}

		if(bottomtype == 2)
		{
			if(table->m_GameLogic.GetCardColor(player->card_array[0]) == table->m_GameLogic.GetCardColor(player->card_array[1]))
				return true;
		}

		return false;
	}
	return false;
}

bool TaskManager::calcBottomValue(Player* player, Table* table, short bbovalue)
{
	if(bbovalue != 0)
	{
		if(table->m_GameLogic.GetCardValue(player->card_array[0]) == bbovalue ||
			table->m_GameLogic.GetCardValue(player->card_array[1]) == bbovalue)
			return true;
		return false;
	}
	return false;
}

bool TaskManager::calcColor(Player* player, Table* table, short bcolor)
{
	if(bcolor != 0)
	{
		for(int i = 0; i < 2; ++i)
		{
			if((player->card_array[i]>>4) == bcolor-1)
				return true;
		}

		if(table->m_bCurrRound == 2)
		{
			for(int i = 0; i < 3; ++i)
			{
				if((table->m_cbCenterCardData[i]>>4) == bcolor-1)
					return true;
			}
		}

		if(table->m_bCurrRound == 3)
		{
			for(int i = 0; i < 4; ++i)
			{
				if((table->m_cbCenterCardData[i]>>4) == bcolor-1)
					return true;
			}
		}

		if(table->m_bCurrRound >= 4)
		{
			for(int i = 0; i < 5; ++i)
			{
				if((table->m_cbCenterCardData[i]>>4) == bcolor-1)
					return true;
			}
		}
		return false;
	}
	return false;
}

bool TaskManager::calcPlayerComplete(Player* player, Table* table)
{
	if(player == NULL || table == NULL)
		return false;
	if(player->m_pTask == NULL)
		return false;
	if(player->m_lFinallGetCoin <= 0)
		return false;
	//前两位为只赢取本局
	short winflag = player->m_pTask->taskid & 0x03;
	//牌型
	short cardtype = player->m_pTask->taskid >> 2 & 0x0F;
	//牌的个数
	short cardnum = player->m_pTask->taskid >> 6 & 0x07;
	//牌型中有的牌值
	short cardvalue = player->m_pTask->taskid >> 9 & 0x0F;
	//底牌牌型
	short bottomtype = player->m_pTask->taskid >> 13 & 0x07;
	//底牌牌值
	short bottomvalue = player->m_pTask->taskid >> 16 & 0x0F;
	//花色
	short cardcolor = player->m_pTask->taskid >> 20 & 0x07;
	_LOG_DEBUG_("winflag:%d cardtype:%d cardnum:%d cardvalue:%d bottomtype:%d bottomvalue:%d cardcolor:%d\n",
		winflag, cardtype, cardnum, cardvalue, bottomtype, bottomvalue, cardcolor);

	bool bwin, btype, bvalue, bbotype, bbovalue, bcolor;
	bwin = btype = bvalue = bbotype = bbovalue = bcolor = false;

	bwin = calcWinOrAllin(player, table, winflag);
	btype = calcCardType(player, table, cardtype);
	bvalue = calcCardValue(player, table, cardnum, cardvalue);
	bbotype = calcBottomType(player, table, bottomtype);
	bbovalue = calcBottomValue(player, table, bottomvalue);
	bcolor = calcColor(player, table, cardcolor);

	if(winflag != 0 && !bwin)
		return false;
	if(cardtype != 0 && !btype)
		return false;
	if(cardnum != 0 && cardvalue != 0 && !bvalue)
		return false;
	if(bottomtype != 0 && !bbotype)
		return false;
	if(bottomvalue != 0 && !bbovalue)
		return false;
	if(cardcolor != 0 && !bcolor)
		return false;

	return true;
}


int TaskManager::getNewTask(Player* player)
{
	if(player == NULL)
		return -1;

	redisContext *m_redis = NULL;  
    redisReply *reply = NULL;      
    m_redis = initRedisContext(Configure::getInstance()->m_sServerRedisAddr[WRITE_REDIS_CONF].c_str());
    if (NULL == m_redis)
		return -1;

	_LOG_INFO_("Initredis-connect OK...\n");

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET OTE|%d ntask",player->id));
	int ntask = 0;
    if(reply == NULL)
    {
         _LOG_ERROR_("Redis can't HGET OTE|%d ntask\n",player->id);
		 redisFree(m_redis);
		 return -1;
    }
	else
	{
		if (reply->type == REDIS_REPLY_STRING) 
		{
			ntask = atoi(reply->str);
		}
		else if(reply->type == REDIS_REPLY_NIL) 
		{
			ntask = 0;
		}
		else
		{
			freeReplyObject(reply);
			redisFree(m_redis);
			return -1;
		}
		freeReplyObject(reply);
	}
	if(ntask == 0)
	{	
		redisFree(m_redis);
		return -1;
	}
	redisFree(m_redis);
	return ntask;
}


vector<string> split(const string& src,const string& tok, bool trim , string null_subst )
{
    vector<string> v;
    if( src.empty() || tok.empty() )
        return v;
    string::size_type pre_index = 0, index = 0, len = 0;
    while( (index = src.find_first_of(tok, pre_index)) != string::npos )
    {
         if( (len = index-pre_index)!=0 )
            v.push_back(src.substr(pre_index, len));
        else if(trim==false)
             v.push_back(null_subst);
         pre_index = index+1;
    }
    string endstr = src.substr(pre_index);
     if( trim==false ) v.push_back( endstr.empty()? null_subst:endstr );
     else if( !endstr.empty() ) v.push_back(endstr);
    return v;
}



