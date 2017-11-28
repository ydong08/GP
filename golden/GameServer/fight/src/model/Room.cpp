#include <map>
#include "Logger.h"
#include "Room.h"
#include "BaseClientHandler.h"
#include "Configure.h"
#include "GameCmd.h"
#include "IProcess.h"
#include "AllocSvrConnect.h"

static Room * Instance = NULL;

Room::Room()
{
	this->m_nStatus = 1;
    this->monitor_time    = Configure::getInstance()->monitor_time;
    this->keeplive_time   = Configure::getInstance()->keeplive_time;
	this->max_user_count  = Configure::getInstance()->GetMaxUserNumber(); 
	this->max_table_count = Configure::getInstance()->GetMaxTableNumber();
	for (int i = 0; i < max_table_count; i++)
	{
		
		tables.push_back(new Table(i));
		tables[i]->init();
	}
	for (int i = 0; i < max_user_count; i++)
	{
		players.push_back(new Player());
		players[i]->tid = -1;
		players[i]->m_nSeatID = i;
		players[i]->m_nStatus = STATUS_PLAYER_LOGOUT;
	}
}

Room::~Room()
{
	for (auto it = tables.begin(); it != tables.end(); it++)
	{
		delete (*it);
		*it = NULL;
	}
	for (auto it = players.begin(); it != players.end(); it++)
	{
		delete (*it);
		*it = NULL;
	}
}

Room * Room::getInstance()
{
	if(Instance==NULL)
		Instance  = new Room();
	return Instance;
}

int Room::init()
{
    this->timer.init(this);
	this->startHeartTimer();
	max_count = 0;
	return 0;
}

static Table* myGetAvailableTable(int uid,std::vector<Table*> tables,int max_table_count)
{
	_LOG_WARN_("[WARING] %s\n", "myGetAvailableTable");
	for(int i=0;i<max_table_count ;++i)
	{
		if(tables[i]->isNotFull() && !tables[i]->isUserInTab(uid))
			return tables[i];
	}

	for(int i=0;i<max_table_count ;++i)
	{
		if(tables[i]->isEmpty())
			return tables[i];
	}
	return NULL;
}

Table* Room::getAvailableTable()
{
	for(int i=0;i<max_table_count ;++i)
	{
		if((tables[i]->isNotFull() || tables[i]->isEmpty()) && !tables[i]->isLock())//空桌子未锁定可以用
			return tables[i];
	}
	return NULL;
}

Table* Room::getAvailableTable(Player* player)
{
	if(player==NULL)
		return NULL;

	return myGetAvailableTable(player->id,this->tables,this->max_table_count);
}

Table* Room::getAvailableTable(int table_id)
{
	for(int i = 0; i < max_table_count ;++i)
	{
		if(!tables[i]->isLock() && tables[i]->isEmpty())//空桌子未锁定可以用
			return tables[i];
	}
	return NULL;
}

Table* Room::getAvailableTable(int uid, std::string strip)
{
	Table* pTable = NULL;
	for(int i=0; i < max_table_count ;++i)
	{
		if(tables[i]->isNotFull() && !tables[i]->isUserInTab(uid))
		{
			pTable = tables[i];
			if (pTable->CheckIP(strip))
				return pTable;
		}
	}

	for(int i=0;i<max_table_count ;++i)
	{
		if(tables[i]->isEmpty())//空桌子未预定可以用
			return tables[i];
	}
	ULOGGER(E_LOG_ERROR, uid) << "not found table. strip=" << strip;
	return  NULL;
}

Table* Room::getTable(int table_id)
{
	if(table_id<0 || table_id>this->max_table_count )
		return NULL;

	return tables[table_id];

}

Table* Room::getChangeTable(int changeTid)
{
	for(int i=0;i<max_table_count ;++i)
	{
		if(i != changeTid)
		{
			if(tables[i]->isNotFull() || tables[i]->isEmpty())//空桌子未预定可以用
				return tables[i];
		}
	}
	return NULL;
}

Player* Room::getAvailablePlayer()
{
	for(int i=0;i<max_user_count;++i)
	{
		if(players[i]->m_nStatus == STATUS_PLAYER_LOGOUT)
			return players[i];
	}
	return NULL;

}

Player* Room::getPlayer(int m_nSeatID)
{
	if(m_nSeatID<0 || m_nSeatID>max_user_count)
		return NULL;

	return players[m_nSeatID];
}

Player* Room::getPlayerUID(int uid)
{
	for(int i = 0; i < max_user_count; ++i)
	{
		if(players[i]->id == uid)
			return players[i];
	}

	return NULL;
}

void Room::startHeartTimer()
{
	 timer.startHeartTimer();
}

void Room::stopHeartTimer()
{
	 timer.stopHeartTimer();
}

//======================Roomtimer=============================

void RoomTimer::init(Room *room)
{
    this->room = room;
}

int RoomTimer::ProcessOnTimerOut()
{
	if(room->getStatus()==-1)
	{
		_LOG_INFO_("Svid[%d]:Room Status=[%d],Wait For Player Empty and Close.... CurrUsers=[%d]\n",Configure::getInstance()->m_nServerId, room->getStatus(),room->getCurrUsers());
		if(room->getCurrUsers()==0)
		{
			_LOG_INFO_("Svid[%d]:Room Status=[%d],Player Empty And Close Server\n",Configure::getInstance()->m_nServerId, room->getStatus());
			_LOG_DEBUG_("Server Exit\n");
			exit(0);
		}
	}

	return HeartTimeOut();
}

void RoomTimer::startHeartTimer()
{    
    this->StartTimer(this->room->monitor_time*1000);
}

void RoomTimer::stopHeartTimer()
{
	this->StopTimer();
}

//定时检查各个棋桌中是否有超时的，判断标准为在一定时间内没有收到心跳包
//只要是不在下棋的超时玩家都踢出去
//
int RoomTimer::HeartTimeOut()
{
	_LOG_INFO_("============Room Info============\n");
	//_LOG_INFO_("Version:flower[%s.%s]\n",VERSION,SUBVER);
	int curr_user_count = 0;
	int curr_table_count = 0;
	room->iphone_count = 0;
	room->android_count = 0;
	room->ipad_count = 0;
	room->robot_count = 0;
	int i = 0;
	for(i=0; i< room->getMaxUsers() ;++i)
	{
		Player * player = room->players[i];
		if( !player->isLogout())
		{
			curr_user_count++;
			if(player->source == 1) room->android_count ++;
			else if(player->source == 2) room->iphone_count ++;
			else if(player->source == 3) room->ipad_count ++;
            Table* table = room->getTable(player->tid);
			int differtime = time(NULL) - player->getActiveTime();

            if (differtime > 10 && table)
            {
                IProcess::NotifyPlayerNetStatus(table, player->id, 0, 1);
            }

			if ((!player->isActive()) && (differtime > room->keeplive_time))
			{
				_LOG_WARN_("KickPlayer:player[%d] um_nStatus[%d], Haven't recv the heart beat for [%d] second, more than keeplive_time[%d]\n",
					player->id, player->m_nStatus, differtime, room->keeplive_time);
				if(player->tid != -1)
				{
					if(table)
					{
						IProcess::serverPushLeaveInfo(table, player);
						table->playerLeave(player);
					}
				}
				else
					//player->m_nStatus = STATUS_PLAYER_LOGOUT;
					player->leave();
			}
		}
	}
	for(i=0; i<room->getMaxTables(); ++i)
	{
		Table* table = room->tables[i];
		if(!table->isEmpty())
		{
			curr_table_count++;
		}
		//是私人房则处理房间创建没有人进来的问题
		if(table->m_nType == PRIVATE_ROOM)
		{
			if(table->tableName[0] != '\0')
				AllocSvrConnect::getInstance()->updateTableUserCount(table);
		}
	}

	room->setCurrTables(curr_table_count);
	room->setCurrUsers(curr_user_count);
	_LOG_INFO_("curr_user_count=[%d] max_count=[%d]  max_count_tab=[%d]\n", curr_user_count, room->max_count, room->max_count_tab);
	this->startHeartTimer();
    return 0;
}

