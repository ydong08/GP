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
	this->max_user_count = Configure::getInstance()->max_user < MAX_ROOM_TABLE*GAME_PLAYER ? Configure::getInstance()->max_user : MAX_ROOM_TABLE*GAME_PLAYER;
	this->max_table_count = Configure::getInstance()->max_table < MAX_ROOM_TABLE ? Configure::getInstance()->max_table : MAX_ROOM_TABLE;
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
    ClearDisConnectPlayerObj();

	for(int i=0;i<max_user_count;++i)
	{
        if (players[i].m_nStatus == STATUS_PLAYER_LOGOUT)
        {
            LOGGER(E_LOG_ERROR) << "max user count is " << max_user_count << ", current user count is:"<< curr_user_count++ ;
            return &players[i];
        }
	}

    LOGGER(E_LOG_ERROR) << "max user count is " << max_user_count << ", current user count is:" << curr_user_count << ",  not enough player obj now!!!";
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

int Room::ClearDisConnectPlayerObj()
{
    curr_user_count = 0;
    android_count = 0;
    iphone_count = 0;
    ipad_count = 0;
    robot_count = 0;

    int i = 0;
    Table* table = getTable();
    if (table == NULL)
    {
        LOGGER(E_LOG_ERROR) << "table is NULL";
        return -1;
    }

    for (i = 0; i < getMaxUsers(); ++i)
    {
        Player * player = &players[i];
        if (!player->isLogout())
        {
            curr_user_count++;

            if (player->source == 1)
            {
                android_count++;
            }
            else if (player->source == 2)
            {
                iphone_count++;
            }
            else if (player->source == 3)
            {
                ipad_count++;
            }
            else if (player->source == 30)
            {
                if (player->m_lMoney < 10000)  //清理身上携带金币不足的机器人
                {
                    LOGGER(E_LOG_DEBUG) << "Robot " << player->id << " money less than 100, system kick it out game !";
                    IProcess::serverPushLeaveInfo(table , player);
                    table->playerLeave(player);

                    curr_user_count--;
                    continue;
                }
                else
                {
                    robot_count++;
                }
            }

            int differtime = time(NULL) - player->getActiveTime();
            if (differtime > keeplive_time)
            {
                if (player->id != table->bankeruid && !player->isbankerlist)
                {
                    _LOG_WARN_("KickPlayer:player[%d] um_nStatus[%d], Haven't recv the heart beat for [%d] second, more than keeplive_time[%d]\n" ,
                        player->id , player->m_nStatus , differtime , keeplive_time);
                    if (table)
                    {
                        player->isonline = false;
                        //IProcess::serverPushLeaveInfo(table, player);
                        ULOGGER(E_LOG_INFO , player->id) << "timeout leave";
                        if (player->notBetCoin())
                        {
                            IProcess::serverPushLeaveInfo(table , player);
                            table->playerLeave(player);

                            curr_user_count--;
                        }
                    }
                }
                else
                {
                    _LOG_WARN_("player[%d] um_nStatus[%d], Haven't recv the heart beat for [%d] second, more than keeplive_time[%d], but player is banker or in banker list now .....\n" ,
                        player->id , player->m_nStatus , differtime , keeplive_time);
                }
            }
        }
    }

    LOGGER(E_LOG_DEBUG) << "Clear the player who is long time idle, now the total user count is "<< curr_user_count <<" , robot user count is "<<robot_count;
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
    stopHeartTimer();

    room->ClearDisConnectPlayerObj();

    startHeartTimer();
    return 0;
}

