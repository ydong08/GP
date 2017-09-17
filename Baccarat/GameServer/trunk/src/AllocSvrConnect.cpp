#include <string>
#include "AllocSvrConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "GameCmd.h"
#include "ProtocolServerId.h"
#include "CDLReactor.h"

static AllocSvrConnect* instance = NULL;

AllocSvrConnect* AllocSvrConnect::getInstance()
{
	if(instance==NULL)
	{
		instance = new AllocSvrConnect();
	}
	return instance;
}

AllocSvrConnect::AllocSvrConnect()
{
	
}

AllocSvrConnect::~AllocSvrConnect()
{
	
}

//上报服务器Info
int AllocSvrConnect::reportSeverInfo()
{
	Room* room = Room::getInstance();
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_REPORT);
	ReportPkg.WriteShort(Configure::getInstance()->m_nServerId); 
	ReportPkg.WriteString(Configure::getInstance()->m_sListenIp);
	ReportPkg.WriteShort(Configure::getInstance()->m_nPort); 

	ReportPkg.WriteShort(Configure::getInstance()->m_nLevel); 

	ReportPkg.WriteByte(room->getStatus()); 
	ReportPkg.WriteByte(Configure::getInstance()->m_nWeight);
	
	ReportPkg.WriteInt(room->getMaxUsers());
	ReportPkg.WriteShort(room->getMaxTables()); 
	ReportPkg.WriteInt(room->getCurrUsers());
	Table* tableArray[512] = {0};
	int currTable = 0;
	for(int i=0; i < room->getMaxTables() ;++i)
	{
		Table *table = &room->tables[i];
		if(!table->isEmpty())
		{
			//int tid = (Configure::getInstance()->m_nServerId << 16) | table->id;
			tableArray[currTable++] = table;
		}
	}
	ReportPkg.WriteShort(currTable);
	for(int j = 0; j < currTable; j++)
	{
		Table *table = tableArray[j];
		if (table == NULL)
		{
			continue;
		}
		int playerCnt = table->getPlayerCount(false, true);
		int tid = (Configure::getInstance()->m_nServerId << 16) | table->id;
		ReportPkg.WriteInt(tid);
		ReportPkg.WriteShort(Configure::getInstance()->m_nLevel);
		ReportPkg.WriteShort(playerCnt);
		if (playerCnt > 0)
		{
			for(int i = 0; i < GAME_PLAYER; ++i)
			{
				if(table->player_array[i])
				{
					ReportPkg.WriteInt64(table->player_array[i]->m_lMoney);
					break;
				}
			}
		}
		else 
			ReportPkg.WriteInt64(0);
	}
	ReportPkg.End();
	return Send(&ReportPkg,false);
}

//更新服务器Info：在线人数
int AllocSvrConnect::updateSeverInfo()
{
	Room* room = Room::getInstance();
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_UPDATE);
	ReportPkg.WriteShort(Configure::getInstance()->m_nServerId); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nLevel); 
	ReportPkg.WriteInt(room->getMaxUsers());
	ReportPkg.WriteShort(room->getMaxTables()); 
	ReportPkg.WriteInt(room->getCurrUsers());
	ReportPkg.WriteShort(room->getCurrTables());
	ReportPkg.WriteShort(room->getIPhoneUsers()); 
	ReportPkg.WriteShort(room->getAndroidUsers()); 
	ReportPkg.WriteShort(room->getIPadUsers()); 
	ReportPkg.WriteShort(room->getRobotUsers());
	

	ReportPkg.End();

	//MySqlConnect* connect = MySqlConnect::getInstance();
	//connect->Send( &ReportPkg );
	return Send(&ReportPkg,false);


}

//当桌子上面人数有变化的时候就通知Alloc更新当前桌子的信息
int AllocSvrConnect::updateTableUserCount(Table* table)
{
	if (table == NULL)
	{
		return -1;
	}

	short playerCnt = table->getPlayerCount(false, true);
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_SET_TABLEUSER);
	ReportPkg.WriteShort(Configure::getInstance()->m_nServerId);
	int tid = table->id;
	ReportPkg.WriteInt(tid); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nLevel);
	ReportPkg.WriteShort(playerCnt); 
	if(playerCnt > 0)
	{
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(table->player_array[i])
			{
				ReportPkg.WriteInt64(table->player_array[i]->m_lMoney);
				break;
			}
		}
	}
	else
		ReportPkg.WriteInt64(0);
	ReportPkg.End();
	return Send(&ReportPkg,false);
}

int AllocSvrConnect::updateTableStatus(Table* table)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_SET_TABLE_STATUS);
	ReportPkg.WriteShort(Configure::getInstance()->m_nServerId);
	int tid = Configure::getInstance()->m_nServerId<< 16 | table->id;
	ReportPkg.WriteInt(tid); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nLevel);
	ReportPkg.WriteShort(table->m_nStatus); 
	ReportPkg.End();
	return Send(&ReportPkg,false);
}

//User离开上报
int AllocSvrConnect::userLeaveGame(Player* player, int uid)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_LEAVE);
	ReportPkg.WriteInt(player ? player->id : uid); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nServerId); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nLevel); 
	ReportPkg.WriteInt(player ? player->m_nWin:0); 
	ReportPkg.WriteInt(player ? player->m_nLose:0); 
	ReportPkg.WriteInt(player ? player->m_nRunAway:0);
	ReportPkg.WriteInt(player ? player->m_nTie:0);
	ReportPkg.End();
	return Send(&ReportPkg,false);
}
//User进入上报
int AllocSvrConnect::userEnterGame(Player* player)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_ENTER);
	ReportPkg.WriteInt(player->id); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nServerId); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nLevel); 
	ReportPkg.WriteInt(player->m_nWin); 
	ReportPkg.WriteInt(player->m_nLose); 
	ReportPkg.WriteInt(player->m_nRunAway); 
	ReportPkg.WriteInt(player->m_nTie); 
	ReportPkg.End();
	return Send(&ReportPkg,false);
}
//User状态上报
int AllocSvrConnect::userUpdateStatus(Player* player,short nm_nStatus)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_USER_STATUS);
	ReportPkg.WriteInt(player->id); 
	ReportPkg.WriteShort(nm_nStatus); 
	int tid = player->tid; 
	ReportPkg.WriteInt(tid); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nServerId); 
	ReportPkg.WriteShort(Configure::getInstance()->m_nLevel);

	ReportPkg.End();
	return Send(&ReportPkg,false);
}

//大厅返回Report 设置结果是否成功 
int AllocSvrConnect::processReportResponse(InputPacket* pPacket)
{
	_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "==>[process Report Response] CMD=[0x%04x] \n", pPacket->GetCmdType());

	short errorno = pPacket->ReadShort();
	std::string errormsg = pPacket->ReadString();

	_LOG_DEBUG_( "[DATA Recv ] errorno=[%d] \n",errorno );
	_LOG_DEBUG_( "[DATA Recv ] errormsg=[%s] \n",errormsg.c_str() );

	return 0;
}

//大厅返回Update 设置结果是否成功 
int AllocSvrConnect::processUpdateResponse(InputPacket* pPacket)
{
	_LOG_DEBUG_( "==>[process Update Response] CMD=[0x%04x] \n", pPacket->GetCmdType());

	short errorno = pPacket->ReadShort();
	std::string errormsg = pPacket->ReadString();

	_LOG_DEBUG_( "[DATA Recv ] errorno=[%d] \n",errorno );
	_LOG_DEBUG_( "[DATA Recv ] errormsg=[%s] \n",errormsg.c_str() );

	if(errorno==-1)
	{
		this->reportSeverInfo();
	}

	return 0;
}

//大厅返回SeverInfo
int AllocSvrConnect::processSeverInfoResponse(InputPacket* pPacket)
{
		_LOG_DEBUG_("\n");
	_LOG_DEBUG_( "==>[process SeverInfo Response] CMD=[0x%04x] \n", pPacket->GetCmdType());

	short errorno = pPacket->ReadShort();
	std::string errormsg = pPacket->ReadString();

	if(errorno!=0)
	{
		_LOG_DEBUG_( "[DATA Recv ] errorno=[%d] \n",errorno );
		_LOG_DEBUG_( "[DATA Recv ] errormsg=[%s] \n",errormsg.c_str() );
		return 0;
	}	
	return 0;
		
}

int AllocSvrConnect::processSetServerStatus(InputPacket* inputPacket)
{
	int cmd = inputPacket->GetCmdType();
	short subcmd =  inputPacket->GetSubCmd();
	short svid = inputPacket->ReadShort();
	char  m_nStatus = inputPacket->ReadByte();
	_LOG_DEBUG_( "==>[Set GameServer Status] CMD=[0x%04x] \n",cmd);
	_LOG_DEBUG_( "[DATA Recv ] svid=[%d]\n", svid);
	_LOG_DEBUG_( "[DATA Recv ] m_nStatus=[%d]\n", m_nStatus);

	if(Configure::getInstance()->m_nServerId != svid)
	{
		_LOG_ERROR_( "[DATA Recv] Configure m_nServerId[%d] svid=[%d]\n",Configure::getInstance()->m_nServerId, svid);
		return -1;
	}

	if(subcmd==0)
	{
		Room* room = Room::getInstance();
		room->setStatus(m_nStatus);
	}
	else
		Configure::getInstance()->m_nWeight = m_nStatus;
	return 0;
}

int AllocSvrConnect::processCreatePrivateTable(InputPacket* pPacket)
{
	
	return 0;
}

int AllocSvrConnect::processOtherMsg(InputPacket* pPacket)
{
	switch (pPacket->GetCmdType())
	{
	case CLIENT_CREATE_PRIVATE: this->processCreatePrivateTable(pPacket); break;
	default:
		LOGGER(E_LOG_ERROR) << "[UnKnow CMD]  cmd = " << TOHEX(pPacket->GetCmdType());
	}
	return 0;
}
