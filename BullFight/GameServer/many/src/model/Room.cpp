#include <map>
#include "Logger.h"
#include "Room.h"
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
	curr_table_active = 1;
	max_count_tab = 1;
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
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		players[i].m_nSeatID = i;
		players[i].init();
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

Table* Room::getTable()
{
	return &tables[0];
}

Table* Room::getChangeTable(int changeTid)
{
	for(int i=0;i<max_table_count ;++i)
	{
		if(i != changeTid)
		{
			if((tables[i].isNotFull() || tables[i].isEmpty()))//空桌子未预定可以用
				return &tables[i];
		}
	}
	return NULL;
}

Player* Room::getAvailablePlayer()
{
	for(int i=0;i<max_user_count;++i)
	{
		if(players[i].m_nStatus == STATUS_PLAYER_LOGOUT)
			return &players[i];
	}
	return NULL;

}

Player* Room::getPlayer(int m_nSeatID)
{
	if(m_nSeatID<0 || m_nSeatID>max_user_count)
		return NULL;

	return &players[m_nSeatID];
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
			_LOG_INFO_("Svid[%d]:Room Status=[%d],Player Empty And Close Server\n",Configure::getInstance()->m_nServerId,room->getStatus());
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
	int curr_user_count = 0;
	//int curr_table_count = 0;
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
			else if(player->source == 30) room->robot_count++;
			
			int differtime = time(NULL) - player->getActiveTime();
			if (differtime > room->keeplive_time)
			{
				_LOG_WARN_("KickPlayer:player[%d] um_nStatus[%d], Haven't recv the heart beat for [%d] second, more than keeplive_time[%d]\n",
					player->id, player->m_nStatus, differtime, room->keeplive_time);
				Table* table = room->getTable();
				if(table)
				{
					player->isonline = false;
					//IProcess::serverPushLeaveInfo(table, player);
					if(player->notBetCoin())
					{
						IProcess::serverPushLeaveInfo(table, player);
						table->playerLeave(player);
					}
				}

			}
		}
	}
	room->setCurrUsers(curr_user_count);
	_LOG_INFO_("curr_user_count=[%d] max_count=[%d]\n",curr_user_count,room->max_count);
	this->startHeartTimer();
    return 0;
}

