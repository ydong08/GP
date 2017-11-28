#include <string>
#include "AllocSvrConnect.h"
#include "Logger.h"
#include "Configure.h"
#include "ClientHandler.h"
#include "GameCmd.h"
#include "MySqlConnect.h"
#include "ProcessFactory.h"
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
	reportTimer = new ReportTimer();	
	reportTimer->init(this);
}

AllocSvrConnect::~AllocSvrConnect()
{
	_LOG_ERROR_( "[AllocSvrConnect::~AllocSvrConnect] AllocSvrConnect was delete\n" );
}

int AllocSvrConnect::connect(const char* ip, short port)
{
	//启动定时器
	reportTimer->StartTimer(30*1000);

		//建立连接
	strcpy(this->ip, ip);
	this->port = port;
	this->handler = new SocketHandle(this);
	return CDLReactor::Instance()->Connect(handler, ip, port);
}

//重连接
int AllocSvrConnect::reconnect()
{
	this->handler = new SocketHandle(this);

	return CDLReactor::Instance()->Connect(handler,ip, port);
}

//上报服务器Info
int AllocSvrConnect::reportSeverInfo()
{
	Room* room = Room::getInstance();
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_REPORT);
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.WriteString(Configure::getInstance()->report_ip); 
	ReportPkg.WriteShort(Configure::getInstance()->port); 

	ReportPkg.WriteShort(Configure::getInstance()->level); 

	ReportPkg.WriteByte(room->getStatus()); 
	ReportPkg.WriteByte(Configure::getInstance()->server_priority); 
	
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
			int tid = (Configure::getInstance()->server_id << 16) | table->id;
			tableArray[currTable++] = table;
		}
	}
	ReportPkg.WriteShort(currTable);
	for(int j = 0; j < currTable; j++)
	{
		Table *table = tableArray[j];
		int tid = (Configure::getInstance()->server_id << 16) | table->id;
		ReportPkg.WriteInt(tid);
		ReportPkg.WriteShort(table->clevel);
		ReportPkg.WriteShort(table->countPlayer);
		if (table->countPlayer > 0)
		{
			for(int i = 0; i < GAME_PLAYER; ++i)
			{
				if(table->player_array[i])
				{
					ReportPkg.WriteInt64(table->player_array[i]->money);
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
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.WriteShort(Configure::getInstance()->level); 
	ReportPkg.WriteInt(room->getMaxUsers());
	ReportPkg.WriteShort(room->getMaxTables()); 
	ReportPkg.WriteInt(room->getCurrUsers());
	ReportPkg.WriteShort(room->getCurrTables());     
	ReportPkg.WriteShort(room->getIPhoneUsers()); 
	ReportPkg.WriteShort(room->getAndroidUsers()); 
	ReportPkg.WriteShort(room->getIPadUsers()); 
	ReportPkg.WriteShort(room->getRobotUsers());
	

	ReportPkg.End();

	MySqlConnect* connect = MySqlConnect::getInstance();
	//connect->Send( &ReportPkg );
	return Send(&ReportPkg,false);


}

//当桌子上面人数有变化的时候就通知Alloc更新当前桌子的信息
int AllocSvrConnect::updateTableUserCount(Table* table)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_SET_TABLEUSER);
	ReportPkg.WriteShort(Configure::getInstance()->server_id);
	int tid = Configure::getInstance()->server_id<< 16 | table->id;
	ReportPkg.WriteInt(tid); 
	ReportPkg.WriteShort(table->clevel); 
	ReportPkg.WriteShort(table->countPlayer); 
	if(table->countPlayer > 0)
	{
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(table->player_array[i])
			{
				ReportPkg.WriteInt64(table->player_array[i]->money);
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
	ReportPkg.WriteShort(Configure::getInstance()->server_id);
	int tid = Configure::getInstance()->server_id<< 16 | table->id;
	ReportPkg.WriteInt(tid); 
	ReportPkg.WriteShort(table->clevel); 
	ReportPkg.WriteShort(table->status); 
	ReportPkg.End();
	return Send(&ReportPkg,false);
}

//User离开上报
int AllocSvrConnect::userLeaveGame(Player* player, int uid)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_LEAVE);
	ReportPkg.WriteInt(player ? player->id : uid); 
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.WriteShort(Configure::getInstance()->level);
	ReportPkg.WriteInt(player ? player->nwin:0);
	ReportPkg.WriteInt(player ? player->nlose:0);
	ReportPkg.WriteInt(player ? player->nrunaway:0);
	ReportPkg.WriteInt(player ? player->ntie:0);
	ReportPkg.End();
	return Send(&ReportPkg,false);
}
//User进入上报
int AllocSvrConnect::userEnterGame(Player* player)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_ENTER);
	ReportPkg.WriteInt(player->id); 
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.WriteShort(Configure::getInstance()->level); 
	ReportPkg.WriteInt(player->nwin);
	ReportPkg.WriteInt(player->nlose);
	ReportPkg.WriteInt(player->nrunaway);
	ReportPkg.WriteInt(player->ntie);
	ReportPkg.End();
	return Send(&ReportPkg,false);
}
//User状态上报
int AllocSvrConnect::userUpdateStatus(Player* player,short nstatus)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_USER_STATUS);
	ReportPkg.WriteInt(player->id); 
	ReportPkg.WriteShort(nstatus); 
	int tid = Configure::getInstance()->server_id<< 16 | player->tid; 
	ReportPkg.WriteInt(tid); 
	ReportPkg.WriteShort(Configure::getInstance()->server_id); 
	ReportPkg.WriteShort(Configure::getInstance()->level);

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
	char  status = inputPacket->ReadByte();
	_LOG_DEBUG_( "==>[Set GameServer Status] CMD=[0x%04x] \n",cmd);
	_LOG_DEBUG_( "[DATA Recv ] svid=[%d]\n", svid);
	_LOG_DEBUG_( "[DATA Recv ] status=[%d]\n", status);

	if(Configure::getInstance()->server_id != svid)
	{
		_LOG_ERROR_( "[DATA Recv] Configure server_id[%d] svid=[%d]\n",Configure::getInstance()->server_id, svid);
		return -1;
	}

	if(subcmd==0)
	{
		Room* room = Room::getInstance();
		room->setStatus(status);
	}
	else
		Configure::getInstance()->server_priority = status;
	return 0;
}

int AllocSvrConnect::processCreatePrivateTable(InputPacket* pPacket)
{
	int cmd = pPacket->GetCmdType();

	int uid = pPacket->ReadInt();
	int tid = pPacket->ReadInt();
	//int MaxCount = pPacket->ReadShort();
	string tableName = pPacket->ReadString();
	short nhaspassed = pPacket->ReadShort();
	string password = pPacket->ReadString();

	short svid = tid >> 16;
	short realTid = tid & 0x0000FFFF;
	Room* room = Room::getInstance();
	_LOG_INFO_("==>[processCreatePrivateTable] [0x%04x] UID=[%d] Source=[%d]\n", cmd, uid);
	_LOG_DEBUG_("[DATA Parse] uid=[%d]\n", uid);
	_LOG_DEBUG_("[DATA Parse] tid=[%d] realTid=[%d]\n", tid, realTid);
	//_LOG_DEBUG_("[DATA Parse] MaxCount=[%d]\n", MaxCount);
	_LOG_DEBUG_("[DATA Parse] roomName=[%s]\n", tableName.c_str());
	_LOG_DEBUG_("[DATA Parse] nhaspassed=[%d]\n", nhaspassed);
	_LOG_DEBUG_("[DATA Parse] password=[%s]\n", password.c_str());

	Table* table = room->getTable(realTid);

	if(table == NULL)
	{
		_LOG_ERROR_("[processCreatePrivateTable] uid=[%d] tid=[%d] realTid=[%d] Table is NULL\n",uid, tid, realTid);
		return 0;
	}

	if(!table->isEmpty())
	{
		_LOG_ERROR_("[processCreatePrivateTable] uid=[%d] tid=[%d] realTid=[%d] Table is Not Empty countPlayer[%d]\n",
			uid, tid, realTid, table->countPlayer);
		return 0;
	}
	
	table->owner = uid;
	//table->maxCount = MaxCount;
	table->haspasswd = (nhaspassed==1) ? true : false;
	strcpy(table->tableName,tableName.c_str());
	strncpy(table->password,password.c_str(), sizeof(table->password));
	table->password[sizeof(table->password) - 1] = '\0';
	
	return 0;
}

int AllocSvrConnect::trumptComTasToUser(short type, const char* msg, short pid)
{
	OutputPacket ReportPkg;
	ReportPkg.Begin(GMSERVER_CMD_TRUMPT);
	ReportPkg.WriteShort(type); 
	ReportPkg.WriteShort(GAME_ID); 	//GameID 
	ReportPkg.WriteString(msg); 
	ReportPkg.WriteShort(pid); 	//GameID 
	ReportPkg.End();
	return Send(&ReportPkg,false);
}

//========================SocketHandle=============================================
SocketHandle::SocketHandle(AllocSvrConnect* allocsvr)
{
	this->allocsvr = allocsvr;
}

int SocketHandle::OnConnected()
{
	_LOG_INFO_("Connected alloc server. \n");
	this->allocsvr->setActive (true);
	if(this->allocsvr->reportSeverInfo()!=0)
	{
		_LOG_ERROR_( "[AllocSvrConnect::OnConnected] Report SeverInfo ERROR\n" );
	}
	else
		this->allocsvr->updateSeverInfo();
	return 0;
}

// 
int SocketHandle::OnClose()
{
	_LOG_ERROR_( "[AllocSvrConnect::OnClose] AllocSvrConnect DisConnect\n" );
	this->allocsvr->setActive (false);
	return 0;
}

//  
int SocketHandle::OnPacketComplete(InputPacket* pPacket)
{
	/*if (pPacket->CrevasseBuffer() == -1)
	{
		_LOG_ERROR_( "[DATA CrevasseBuffer ERROR] data decrypt error \n" );
		return -1;
	}*/

	switch (pPacket->GetCmdType())
	{
		case GMSERVER_REPORT:   this->allocsvr->processReportResponse(pPacket); break;
		case GMSERVER_UPDATE:   this->allocsvr->processUpdateResponse(pPacket); break;
		case GMSERVER_SET_STATUS: this->allocsvr->processSetServerStatus(pPacket); break;
		case CLIENT_CREATE_PRIVATE: this->allocsvr->processCreatePrivateTable(pPacket); break;
		default:
			_LOG_ERROR_( "[UnKnow CMD]  cmd=[0x%04x] \n" ,pPacket->GetCmdType());
	}
	return 0;
}
//====================================================================


//=============================ReportTimer===============================

int ReportTimer::ProcessOnTimerOut()
{
	if(allocsvr && allocsvr->isActive())
	{
		_LOG_DEBUG_( "***ReportTimer:Report GameServer Info To AllocSvr\n" );
		allocsvr->updateSeverInfo();
	}
	else
	{
		_LOG_ERROR_( "***ReportTimer:AllocSvrConnect Need ReConnect......\n" );
		allocsvr->reconnect();
	}
	StartTimer(30*1000);
	return 0;
}


