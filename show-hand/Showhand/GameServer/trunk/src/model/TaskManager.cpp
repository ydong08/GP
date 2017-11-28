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

int TaskManager::init()
{
	if(loadTask()!=0)
		return -1;
	return 0;
}

int TaskManager::loadTask()
{
	redisContext *m_redis = NULL;  
    redisReply *reply = NULL;      
    m_redis = Util::initRedisContext(Configure::getInstance()->redis_ip , Configure::getInstance()->redis_port);
    if (NULL == m_redis)
		return -1;
  
	_LOG_INFO_("Initredis-connect OK...\n");

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGETALL task:%d",Configure::getInstance()->level));
    if(reply == NULL)
    {
         _LOG_ERROR_("Redis can't get task:%d\n",Configure::getInstance()->level);
		 redisFree(m_redis);
		 return -1;
    }
	else
	{
		if (reply->type == REDIS_REPLY_ARRAY) 
		{
			if(reply->elements==0){
				_LOG_ERROR_("Can't get task:%d,task maybe not add\n",Configure::getInstance()->level);
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
					if(vlist.size() == 4)
					{
						//printf("vlist[0]:%s vlist[1]:%s\n", vlist[0].c_str(), vlist[1].c_str());
						strncpy(taskarry[countTask].taskIOSname,vlist[0].c_str(), sizeof(taskarry[countTask].taskIOSname));
						taskarry[countTask].taskIOSname[sizeof(taskarry[countTask].taskIOSname) - 1] = '\0';
						strncpy(taskarry[countTask].taskANDname,vlist[1].c_str(), sizeof(taskarry[countTask].taskANDname));
						taskarry[countTask].taskANDname[sizeof(taskarry[countTask].taskANDname) - 1] = '\0';
						taskarry[countTask].ningotlow = atoi(vlist[2].c_str());
						taskarry[countTask].ningothigh = atoi(vlist[3].c_str());
						//简单的任务列表
						if(taskarry[countTask].taskid == 4 || taskarry[countTask].taskid == 51380224 || taskarry[countTask].taskid == 68157440
							|| taskarry[countTask].taskid == 34603008 || taskarry[countTask].taskid == 17825792 || taskarry[countTask].taskid == 1048577)
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
		printf("%ld:%s|%s|%d----%d\n", taskarry[i].taskid, taskarry[i].taskIOSname, taskarry[i].taskANDname, taskarry[i].ningotlow, taskarry[i].ningothigh);
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

bool TaskManager::calcPlayerComplete(Task* task, Player* player, Table* table)
{
	if(task == NULL || player == NULL)
		return false;
	if(player->finalgetCoin <= 0)
		return false;
	if(task->taskid == 0)
		return false;
	char cardbuff[64]={0};
	for(int j = 0; j < 5; ++j)
		sprintf(cardbuff+5*j, "0x%02x ",player->card_array[j]);
	_LOG_INFO_("player:[%d] card:[%s] ante:[%ld] getcoin:[%ld] taskid:[%ld] taskname:[%s] getingot:[%d]\n", player->id, cardbuff, 
		table->ante, player->finalgetCoin, task->taskid, player->source == 1 ? player->task1->taskANDname : player->task1->taskIOSname, player->ningot);

	short winflag = task->taskid & 3;
	//前面六位为牌型
	short cardtype = task->taskid >> 2 & 0077;
	if(cardtype < 0 || cardtype > 9)
		return false;
	//紧接着四位为牌的数字大小
	short cardvalue = task->taskid >> 8 & 0x0F;
	if(cardvalue < 0 || (cardvalue > 0 && cardvalue < 8) || cardvalue > 14)
		return false;
	//紧接着四位为底牌的数字大小
	short cardfvalue = task->taskid >> 12 & 0x0F;
	if(cardfvalue < 0 || (cardfvalue > 0 && cardfvalue < 8) || cardfvalue > 14)
		return false;
	//紧接着四位为第二张牌的数字大小
	short cardsvalue = task->taskid >> 16 & 0x0F;
	if(cardsvalue < 0 || (cardsvalue > 0 && cardsvalue < 8) || cardsvalue > 14)
		return false;
	//紧接着四位为操作类型（吓跑或者梭哈）
	short optype = task->taskid >> 20 & 0x0F;
	if(optype < 0 || optype > 3)
		return false;
	//紧接着四位为牌的花色
	short cardcolor = task->taskid >> 24 & 0x0F;
	if(cardcolor < 0 || cardcolor > 4)
		return false;
	//紧接着四位为底注的倍数
	unsigned short mul = task->taskid >> 28 & 0x0FFFF;
	_LOG_DEBUG_("winflag:%d cardtype:%d cardvalue:%d cardfvalue:%d cardsvalue:%d optype:%d cardcolor:%d mul:%d\n",
		winflag, cardtype, cardvalue, cardfvalue, cardsvalue, optype, cardcolor, mul);

	if(winflag != 0)
	{
		if(winflag != 1)
			return false;
	}

	if(cardtype != 0)
	{
		if(player->finalcardvalue == -1)
		{
			short type = table->GetPlayerCardKind(player->card_array, 0, table->currRound>4 ? 4 : table->currRound);
			if(type == 0)
			{
				//散牌
				if(cardtype != 9)
					return false;
			}
			if(type != cardtype)
				return false;
		}
		else
		{
			if(player->finalcardvalue == 0)
			{
				//散牌
				if(cardtype != 9)
					return false;
			}
			else if (player->finalcardvalue != cardtype)
			{
				return false;
			}
		}
	}

	if(cardvalue != 0)
	{
		//当牌型有要求
		if(cardtype != 0)
		{
			if(cardtype == DUI_ZI || cardtype == LAING_DUI)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
						num++;
				}
				if (num != 2)
					return false;
			}
			if(cardtype == SAN_TIAO)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
						num++;
				}
				if (num != 3)
					return false;
			}
			if(cardtype == SHUN_ZI)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
						num++;
				}
				if (num != 1)
					return false;
			}
			if(cardtype == TONG_HUA)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
						num++;
				}
				if (num != 1)
					return false;
			}
			if(cardtype == HU_LU)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
						num++;
				}
				if (num != 3)
					return false;
			}
			if(cardtype == TIE_ZHI)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
						num++;
				}
				if (num != 4)
					return false;
			}

			if(cardtype == TONG_HUA_SHUN)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
					{
						num++;
					}
				}
				if (num != 1)
					return false;
			}
			//散牌
			if(cardtype == 9)
			{
				int num = 0;
				for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
				{
					if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
						num++;
				}
				if (num != 1)
					return false;
			}
		}
		//牌型没有要求
		else
		{
			int num = 0;
			for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
			{
				if((table->m_GameLogic.GetCardValoe(player->card_array[i])) == cardvalue)
					num++;
			}
			if (num == 0)
				return false;
		}
	}

	if(cardfvalue != 0)
	{
		if(cardfvalue != (table->m_GameLogic.GetCardValoe(player->card_array[0])))
			return false;
	}

	if(cardsvalue != 0)
	{
		if(cardsvalue != (table->m_GameLogic.GetCardValoe(player->card_array[1])))
			return false;
	}

	if(optype != 0)
	{
		if(optype & 1)
		{
			if(!player->hasallin)
				return false;
		}
		if(optype & 2)
		{
			if(!table->isthrowwin)
				return false;
		}
	}

	if(cardcolor != 0)
	{
		if(cardcolor == 4)
		{
			int num = 0;
			for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
			{
				if((table->m_GameLogic.GetHuaKind(player->card_array[i])) == FANG_KUAI)
					num++;
			}
			if(num == 0)
				return false;
		}
		if(cardcolor == MEI_HUA)
		{
			int num = 0;
			for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
			{
				if((table->m_GameLogic.GetHuaKind(player->card_array[i])) == MEI_HUA)
					num++;
			}
			if(num == 0)
				return false;
		}
		if(cardcolor == HONG_TAO)
		{
			int num = 0;
			for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
			{
				if((table->m_GameLogic.GetHuaKind(player->card_array[i])) == HONG_TAO)
				{
					num++;
				}
			}
			if(num == 0)
				return false;
		}
		if(cardcolor == HEI_TAO)
		{
			int num = 0;
			for(int i = 0; i <= (table->currRound>4 ? 4 : table->currRound); ++i)
			{
				if((table->m_GameLogic.GetHuaKind(player->card_array[i])) == HEI_TAO)
					num++;
			}
			if(num == 0)
				return false;
		}
	}

	if(mul != 0)
	{
		if(player->finalgetCoin < mul*table->ante)
			return false;
	}

	return true;
}


int TaskManager::setPlayerComplete(Player* player)
{
	if(player == NULL || player->task1 == NULL)
		return -1;

	redisContext *m_redis = NULL;  
    redisReply *reply = NULL;      
    m_redis = Util::initRedisContext(Configure::getInstance()->redis_tip , Configure::getInstance()->redis_tport);
    if (NULL == m_redis)   //连接redis
		return -1;

	_LOG_INFO_("Initredis-connect OK...\n");

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "LINDEX LT:%d 0",player->id));
	int ncompleteCount = 0;
    if(reply == NULL)
    {
         _LOG_ERROR_("Redis can't LINDEX LT:%d 0\n",player->id);
		 redisFree(m_redis);
		 return -1;
    }
	else
	{
		if (reply->type == REDIS_REPLY_STRING) 
		{
			vector<std::string> vlist = split(std::string(reply->str), "|", true, "");
			if(vlist.size() == 3)
			{
				ncompleteCount = atoi(vlist[1].c_str());
			}
		}
		else if(reply->type == REDIS_REPLY_NIL) 
		{
			ncompleteCount = 0;
		}
		else
		{
			freeReplyObject(reply);
			redisFree(m_redis);
			return -1;
		}
		freeReplyObject(reply);
	}

	if(ncompleteCount >= 50)
	{
		reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "RPOP LT:%d",player->id));
		if(reply)
		{
			freeReplyObject(reply);
		}
	}

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "LPUSH LT:%d %ld|%d|%d",player->id, player->task1->taskid, ncompleteCount+1, time(NULL)));
	if(reply)
	{
		freeReplyObject(reply);
	}

	redisFree(m_redis);
	return 0;
}

int TaskManager::getNewTask(Player* player)
{
	if(player == NULL)
		return -1;

	if(Configure::getInstance()->level == 5)
		return 0;

	redisContext *m_redis = NULL;  
    redisReply *reply = NULL;      
    m_redis = Util::initRedisContext(Configure::getInstance()->redis_nip , Configure::getInstance()->redis_nport);
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

//返回 1 说明完成初级场局数任务 2 中级场 3 高级场
int TaskManager::setNewTask(Player* player, int &boardtask)
{
	if(player == NULL)
		return -1;

	if(Configure::getInstance()->level == 5 || Configure::getInstance()->level == 4)
		return 0;

	int ntask = player->boardTask;
	
	int nlow = ntask & 0x00FF;
	int nmid = ntask>>8 & 0x00FF;
	int nhigh = ntask>>16 & 0x00FF;
	int ncomplete = ntask >> 24;
	int nret = 0;
	if(Configure::getInstance()->level == 1)
	{
		if(ncomplete & 1)
		{
			return 0;
		}
		else
		{
			nlow++;
			if(nlow >= player->coincfg.playCount1)
			{
				ncomplete |= 1;
				nret = 1;
			}
		}
	}

	if(Configure::getInstance()->level == 2)
	{
		if(ncomplete & 2)
		{
			return 0;
		}
		else
		{
			nmid++;
			if(nmid >= player->coincfg.playCount2)
			{
				ncomplete |= 2;
				nret = 2;
			}
		}
	}

	if(Configure::getInstance()->level == 3)
	{
		if(ncomplete & 4)
		{
			return 0;
		}
		else
		{
			nhigh++;
			if(nhigh >= player->coincfg.playCount3)
			{
				ncomplete |= 4;
				nret = 3;
			}
		}
	}

	int newtask = ncomplete<<24 | nhigh<<16 | nmid<<8 | nlow;
	boardtask = newtask;

	redisContext *m_redis = NULL;  
    redisReply *reply = NULL;      
    m_redis = Util::initRedisContext(Configure::getInstance()->redis_nip , Configure::getInstance()->redis_nport);
    if (NULL == m_redis)
		return -1;

	_LOG_INFO_("Initredis-connect OK...\n");

	_LOG_DEBUG_("newtask:%d ncomplete:%d nhigh:%d nmid:%d nlow:%d \n", newtask, ncomplete, nhigh, nmid, nlow );
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HSET OTE|%d ntask %d",player->id,newtask));
	if(reply)
	{
		freeReplyObject(reply);
	}

	redisFree(m_redis);
	return nret;
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



