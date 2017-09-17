#include <stdlib.h>
#include <string.h>
#include <map>
#include "ClientHandler.h"
#include "ServerManager.h"
#include "AllocManager.h"
//#include "RedisConnector.h"
#include "GameCmd.h"
#include "Configure.h"
#include "Logger.h"
#include "Protocol.h"
using namespace std;


static bool IsHundredMode(int level)
{
	return (level == E_LEVEL_HUNDRED || GAME_TYPE == E_BULL_2017) ? true : false;
}


static AllocManager* instance = NULL;

AllocManager* AllocManager::getInstance()
{
	if(instance==NULL)
	{
		instance = new AllocManager();
	}
	return instance;
}

int AllocManager::addTable(int tid, short svid, short level, short usercount, int64_t money)
{
	if(usercount < 0 || usercount > Configure::instance()->numplayer)
	{
		_LOG_ERROR_("this table[%d] usercount[%d] to large or to small\n", tid, usercount);
		return -1;
	}
	GameTable *pTable = getTable(tid);
	if(pTable != NULL)
	{
		//如果当前桌子人数和上报人数不同，那就更新以usercount和键值的列表
		if(pTable->m_nUserCount != usercount)
		{
			if(!IsHundredMode(Configure::instance()->m_nLevel))
			{
				TableListMap::iterator iterTable = _tableUserCountMap[pTable->m_nUserCount].find(tid);
				//删除先前位置的信息
				if(iterTable != _tableUserCountMap[pTable->m_nUserCount].end())
				{
					_tableUserCountMap[pTable->m_nUserCount].erase(iterTable);
				}
				//添加到新位置
				_tableUserCountMap[usercount][tid] = pTable;
			}

			pTable->m_nUserCount = usercount;
		}
	}
	else
	{
		pTable = new GameTable(tid, svid, level);
		if(pTable != NULL)
		{
			_tablelistMap[tid] = pTable;
			if(!IsHundredMode(Configure::instance()->m_nLevel))
				_tableUserCountMap[usercount][tid] = pTable;
			pTable->m_nUserCount = usercount;
		}
	}
	return 0;
}

GameTable* AllocManager::getTable(int tid)
{
	GameTable *pTable = NULL;
	map<int, GameTable*>::iterator iter = _tablelistMap.find(tid);
	if(iter != _tablelistMap.end())
	{
		pTable = iter->second;
	}
	return pTable;
}

void AllocManager::delTablesInSvid(short svid)
{
	int i = 0;
	TableListMap::iterator it = _tablelistMap.begin();
	while( it != _tablelistMap.end())
	{
		int tid = it->first;
		GameTable *pTable = it->second;
		if(pTable && pTable->m_nServerId == svid)
		{
			if(!IsHundredMode(Configure::instance()->m_nLevel))
			{
				//此处删除_tableUserCountMap中的桌子信息，防止后面出现野指针的bug
				for(i = 0 ; i <= Configure::instance()->numplayer; ++i)
				{
					TableListMap::iterator iter = _tableUserCountMap[i].find(tid);
					if(iter != _tableUserCountMap[i].end())
					{
						_tableUserCountMap[i].erase(iter);
					}
				}
			}
			delete pTable;
			_tablelistMap.erase(it++);
			pTable = NULL;
		}
		else
			it++;
	}
}

GameTable*  AllocManager::getListTable(int count, int tid)
{
	GameTable *pTable = NULL;
	if (count >= MAX_TABLE_USER + 1)
	{
		return NULL;
	}
	TableListMap::iterator it = _tableUserCountMap[count].begin();
	while (it != _tableUserCountMap[count].end())
	{
		GameTable *pTempTable = it->second;
		if(pTempTable != NULL)
		{
			_LOG_DEBUG_("pTempTable:%d num:%d\n",pTempTable->m_nTid, pTempTable->m_nUserCount);
			GameServer *pServer = ServerManager::getInstance()->getServer(pTempTable->m_nServerId);
			if(pServer != NULL)
			{
				//要当前server是开启状态则从这个server中取桌子,并且适应于换桌功能
				if(pServer->status == 1 && pTempTable->m_nTid != tid)
				{
					pTable = pTempTable;
					_tableUserCountMap[count].erase(it++);
					pTable->m_nUserCount = count + 1;
					_tableUserCountMap[pTable->m_nUserCount][pTable->m_nTid] = pTable;
					break;
				}
				else
					it++;
			}
			else
				_tableUserCountMap[count].erase(it++);
		}
	}
	return pTable;
}

bool AllocManager::isaccordwithMoney(int64_t currmoney, int64_t money)
{
	if(currmoney <= 0)
		return true;

	if(money > Configure::instance()->highlimit)
	{
		if(currmoney > Configure::instance()->highlimit)
			return true;
		if(currmoney*Configure::instance()->mul > money)
			return true;
		return false;
	}

	if(currmoney/Configure::instance()->mul < money && money < currmoney*Configure::instance()->mul)
		return true;
	return false;
}

GameTable* AllocManager::getHighTable(short level, int tid, int64_t money)
{
	GameTable *pTable = NULL;
	TableListMap::iterator it = _tablelistMap.begin();
	while (it != _tablelistMap.end())
	{
		GameTable *pTempTable = it->second;
		if(pTempTable != NULL)
		{
			GameServer *pServer = ServerManager::getInstance()->getServer(pTempTable->m_nServerId);
			if(pServer != NULL)
			{
				//要当前server是开启状态则从这个server中取桌子,并且适应于换桌功能,自己钱是这个桌子的钱的10倍之内
				if(pServer->status == 1 && pTempTable->m_nTid != tid && pTempTable->m_nUserCount < Configure::instance()->numplayer
					&& (pTempTable->m_nUserCount == 0 ||isaccordwithMoney(pTempTable->m_Money, money)))
				{
					pTable = pTempTable;
					break;
				}
				else
					it++;
			}
			else
				_tablelistMap.erase(it++);
		}
	}
	if(pTable)
		return pTable;

	GameServer* pserver = ServerManager::getInstance()->getBestServer(level);
	if(pserver != NULL)
	{
		int svid = pserver->svid;
		int newTableId = (svid<<16) + pserver->currTabCount;
		TableListMap::iterator iter =_tablelistMap.find(newTableId);
		if(iter != _tablelistMap.end())
		{
			_LOG_WARN_("this Tid[%d] svid[%d] currTabCount[%d] is allready in tablelistMap\n", newTableId, pserver->svid,pserver->currTabCount);
			pTable = iter->second;
			if(pTable)
				return pTable;
			else
				return NULL;
		}
		pTable = new GameTable(newTableId, pserver->svid, pserver->level);
		if(pTable != NULL)
		{
			ServerManager::getInstance()->addCurrTabCount(pserver->svid);
			_tablelistMap[newTableId] = pTable;
			pTable->m_nUserCount = 1;
			pTable->m_Money = money;
		}
	}
	return pTable;
}

GameTable* AllocManager::getAllocTable(short level, int tid, int64_t money)
{
	GameTable *pTable = NULL;

	//百人牛牛
	if(IsHundredMode(level))
	{
		//找一个最优的server
		GameServer* pserver = ServerManager::getInstance()->getBestServer(level);
		if(pserver != NULL)
		{
			int svid = pserver->svid;
			//一个游戏服务器只有一个房间
			int newTableId = (svid<<16);
			TableListMap::iterator iter =_tablelistMap.find(newTableId);
			if(iter != _tablelistMap.end())
			{
				//_LOG_WARN_("this Tid[%d] svid[%d] currTabCount[%d] is allready in tablelistMap\n", newTableId, pserver->svid,pserver->currTabCount);
				pTable = iter->second;
				return pTable;
			}
			pTable = new GameTable(newTableId, pserver->svid, pserver->level);
			if(pTable != NULL)
			{
				//这里是实例不是存入内存的指针
				ServerManager::getInstance()->addCurrTabCount(pserver->svid);
				_tablelistMap[newTableId] = pTable;
				pTable->m_nUserCount = 1;
			}
		}
		return pTable;
	}

	//翻翻乐
	if( level == E_LEVEL_HUNDRED )
	{
		//找一个最优的server
		GameServer* pserver = ServerManager::getInstance()->getBestServer(level);
		if(pserver != NULL)
		{
			int newTableId = (pserver->svid<<16) + pserver->currTabCount;;
			TableListMap::iterator iter =_tablelistMap.find(newTableId);
			if(iter != _tablelistMap.end())
			{
				//_LOG_WARN_("this Tid[%d] svid[%d] currTabCount[%d] is allready in tablelistMap\n", newTableId, pserver->svid,pserver->currTabCount);
				pTable = iter->second;
				return pTable;
			}

			pTable = new GameTable(newTableId, pserver->svid, pserver->level);
			if(pTable != NULL)
			{
				//这里是实例不是存入内存的指针
				ServerManager::getInstance()->addCurrTabCount(pserver->svid);
				_tablelistMap[newTableId] = pTable;
				pTable->m_nUserCount = 1;
			}
		}
		return pTable;
	}

	int i = 0;
	for(i = Configure::instance()->numplayer - 1 ; i >= 0; --i)
	{
		_LOG_DEBUG_("i:%d numplayer:%d\n", i, Configure::instance()->numplayer);
		pTable = getListTable(i, tid);
		if(pTable != NULL)
			return pTable;
	}

	//找一个最优的server
	GameServer* pserver = ServerManager::getInstance()->getBestServer(level);
	if(pserver != NULL)
	{
		int svid = pserver->svid;
		int newTableId = (svid<<16) + pserver->currTabCount;
		TableListMap::iterator iter =_tablelistMap.find(newTableId);
		if(iter != _tablelistMap.end())
		{
			_LOG_WARN_("this Tid[%d] svid[%d] currTabCount[%d] is allready in tablelistMap\n", newTableId, pserver->svid,pserver->currTabCount);
			//newTableId = (svid<<16) + pserver->currTabCount + 1;
			//iter =_tablelistMap.find(newTableId);
			///if(iter != _tablelistMap.end())
			//{
			//	_LOG_ERROR_("Twice this Tid[%d] svid[%d] currTabCount[%d] is allready in tablelistMap\n", newTableId, pserver->svid,pserver->currTabCount+1);
			pTable = iter->second;
			if(pTable)
				return pTable;
			else
				return NULL;
			//}
		}
		pTable = new GameTable(newTableId, pserver->svid, pserver->level);
		_LOG_DEBUG_("Alloc new table in svid:%d\n", svid);
		if(pTable != NULL)
		{
			//这里是实例不是存入内存的指针
			//pserver->currTabCount++;
			ServerManager::getInstance()->addCurrTabCount(pserver->svid);
			_tablelistMap[newTableId] = pTable;
			pTable->m_nUserCount = 1;
			_tableUserCountMap[1][pTable->m_nTid] = pTable;
		}
	}
	return pTable;
}

void AllocManager::updateTableUserCount(short svid, int tid, short level, short tableUserCount, int64_t money)
{
	if((tid>>16) == svid)//满足serverId与tid匹配
	{
		GameTable *pTable = getTable(tid);
		if(pTable != NULL)
		{
			//非百人场和翻翻乐
			if(!IsHundredMode(Configure::instance()->m_nLevel))
			{
				if(pTable->m_nUserCount != tableUserCount)
				{
					TableListMap::iterator iterTable = _tableUserCountMap[pTable->m_nUserCount].find(tid);
					if(iterTable != _tableUserCountMap[pTable->m_nUserCount].end())
					{
						_tableUserCountMap[pTable->m_nUserCount].erase(iterTable);
					}
				}
				_tableUserCountMap[tableUserCount][tid] = pTable;
			}
			pTable->m_nUserCount = tableUserCount;
		}
		else
		{
			_LOG_INFO_("updateTableUserCount table not exist tid:[%d],serverId:[%hd],level:[%hd] need Create\n", tid, svid, level);
			pTable = new GameTable(tid, svid, level);//游戏server创建的房间
			if(pTable != NULL)
			{
				_tablelistMap[tid] = pTable;
				if(!IsHundredMode(Configure::instance()->m_nLevel))
					_tableUserCountMap[tableUserCount][pTable->m_nTid] = pTable;
				pTable->m_nUserCount = tableUserCount;
			}
		}
	}
}

void AllocManager::setTableStatus(short svid, int tid, short level, short status)
{
	if((tid>>16) == svid)//满足serverId与tid匹配
	{
		GameTable *pTable = getTable(tid);
		if(pTable != NULL)
		{
			pTable->m_nStatus = status;
		}
		else
		{
			_LOG_INFO_("setTableStatus table not exist tid:[%d],serverId:[%hd],level:[%hd] need Create\n", tid, svid, level);
		}
	}
}

GameTable* AllocManager::createPrivateTable(short level)
{
	GameTable *pTable = NULL;
	pTable = getListTable(0);
	if (pTable != NULL)
	{
		return pTable;
	}

	//找一个最优的server
	GameServer* pserver = ServerManager::getInstance()->getBestServer(level);
	if(pserver != NULL)
	{
		int svid = pserver->svid;
		int newTableId = (svid<<16) + pserver->currTabCount;
		TableListMap::iterator iter =_tablelistMap.find(newTableId);
		if(iter != _tablelistMap.end())
		{
			_LOG_WARN_("this Tid[%d] svid[%d] currTabCount[%d] is allready in tablelistMap\n", newTableId, pserver->svid,pserver->currTabCount);
			pTable = iter->second;
			if(pTable)
				return pTable;
			else
				return NULL;
		}
		pTable = new GameTable(newTableId, pserver->svid, pserver->level);
		if(pTable != NULL)
		{
			ServerManager::getInstance()->addCurrTabCount(pserver->svid);
			_tablelistMap[newTableId] = pTable;
			pTable->m_nUserCount = 1;
			_tableUserCountMap[1][pTable->m_nTid] = pTable;
		}
	}
	return pTable;
}

void AllocManager::getPrivateList(TableListMap &list)
{
	int index = 0;
	TableListMap::iterator iter = _tablelistMap.begin();
	for(; iter!= _tablelistMap.end(); iter++)
	{
		GameTable *pTable = iter->second;
		if(pTable != NULL)
		{
			GameServer *pServer = ServerManager::getInstance()->getServer(pTable->m_nServerId);
			//要当前server是开启状态则从这个server中取桌子
			if(pServer != NULL && pServer->status == 1)
			{
				if(pTable->m_nUserCount>0 && pTable->m_nUserCount<= Configure::instance()->numplayer)
				{
					list[pTable->m_nTid] = pTable;		
					index++;
					if(index >= 100)
						break;
				}
			}
		}
	}
}

