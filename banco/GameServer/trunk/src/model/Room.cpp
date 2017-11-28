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
    this->monitor_time = Configure::getInstance()->monitor_time;
    this->keeplive_time = Configure::getInstance()->keeplive_time;
	this->max_user_count =  Configure::getInstance()->max_user < MAX_ROOM_TABLE*GAME_PLAYER ? Configure::getInstance()->max_user   : MAX_ROOM_TABLE*GAME_PLAYER; 
	this->max_table_count =  Configure::getInstance()->max_table < MAX_ROOM_TABLE   ? Configure::getInstance()->max_table  : MAX_ROOM_TABLE;
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
	for(int i=0;i<MAX_ROOM_TABLE ;++i)
	{
		tables[i].id = i;
		tables[i].init();
		for (int j = 0; j < GAME_PLAYER; j++)
		{
			players[GAME_PLAYER * i + j].tid = -1;
			players[GAME_PLAYER * i + j].m_nSeatID = 0;
			players[GAME_PLAYER * i + j].m_nStatus = STATUS_PLAYER_LOGOUT;
			players[GAME_PLAYER * i + j].init();
		}
	}
	return 0;
}

Table* Room::getAvailableTable()
{
	for(int i=0;i<max_table_count ;++i)
	{
		if(tables[i].isNotFull() || tables[i].isEmpty())
			return &tables[i];
	}
	return NULL;
}

Table* Room::getAvailableTable(int table_id)
{
	for(int i = 0; i < max_table_count ;++i)
	{
		if(!tables[i].isLock() && tables[i].isEmpty())//空桌子未锁定可以用
			return &tables[i];
	}
	return NULL;
}

Table* Room::getTable(int table_id)
{
	if(table_id<0 || table_id>this->max_table_count )
		return NULL;

	return &tables[table_id];

}

Table* Room::getChangeTable(int changeTid)
{
	for(int i=0;i<max_table_count ;++i)
	{
		if(i != changeTid)
		{
			if((tables[i].isNotFull() || tables[i].isEmpty()) && !tables[i].isLock())//空桌子未预定可以用
				return &tables[i];
		}
	}
	return NULL;
}

Player* Room::getAvailablePlayer()
{
	for(int i=0;i<max_user_count;++i)
	{
        if (players[i].m_nStatus == STATUS_PLAYER_LOGOUT)
        {
            return &players[i];
        }
	}

    LOGGER(E_LOG_ERROR) << "max user count is "<<max_user_count<<", not enough player obj now!!!";
	return NULL;
}

Player* Room::getPlayerUID(int uid)
{
	for(int i=0;i<max_user_count;++i)
	{
		if(players[i].id == uid)
			return &players[i];
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
		_LOG_INFO_("Svid[%d]:Room Status=[%d],Wait For Player Empty and Close.... CurrUsers=[%d]\n",Configure::getInstance()->m_nServerId,room->getStatus(),room->getCurrUsers());
		if(room->getCurrUsers()==0)
		{
			_LOG_INFO_("Server Exit Svid[%d]:Room Status=[%d],Player Empty And Close Server\n",Configure::getInstance()->m_nServerId,room->getStatus());
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
	int curr_user_count = 0;
	int curr_table_count = 0;
	room->iphone_count = 0;
	room->android_count = 0;
	room->ipad_count = 0;
	room->robot_count = 0;
	int i = 0;
	for(i=0; i< room->getMaxUsers() ;++i)
	{
		Player * player = &room->players[i];
		if( !player->isLogout())
		{
			curr_user_count++;
			if(player->source == 1) room->android_count ++;
			else if(player->source == 2) room->iphone_count ++;
			else if(player->source == 3) room->ipad_count ++;
			
			int differtime = time(NULL) - player->getActiveTime();
			if ((differtime > room->keeplive_time)) {
				_LOG_WARN_("KickPlayer:player[%d] ustatus[%d], Haven't recv the heart beat for [%d] second, more than keeplive_time[%d]\n",
						   player->id, player->m_nStatus, differtime, room->keeplive_time);
				if (!player->isActive())
				{
					if(player->tid != -1)
					{
						Table* table = room->getTable( player->tid );
						if(table)
						{
							IProcess::serverPushLeaveInfo(table, player);
							table->playerLeave(player);
						}
					}
					else
						player->leave();
				}
				/*else if(player->isActive())
                {
                    Table* table = room->getTable( player->tid );
                    if(table)
                    {
                        IProcess::serverPushLeaveInfo(table, player);
                        table->playerLeave(player);
                    }
                }*/
			}
		}
	}
	for(i=0; i<room->getMaxTables(); ++i)
	{
		Table* table = &room->tables[i];
		if(!table->isEmpty())
		{
			curr_table_count++;
		}
	}
	room->setCurrTables(0); //避免可能的越界，alloc回来后0x200消息仍会再次分配
	room->setCurrUsers(curr_user_count);
	_LOG_INFO_("curr_user_count=[%d] max_count=[%d] curr_table_count=[%d] max_count_tab=[%d]\n",curr_user_count,room->max_count,curr_table_count,room->max_count_tab);
	this->startHeartTimer();
    return 0;
}

