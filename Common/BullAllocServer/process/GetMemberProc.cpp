#include "GetMemberProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "AllocManager.h"
#include "BackConnect.h"
#include "Configure.h"
#include "ErrorMsg.h"
#include "ServerManager.h"
//#include "UdpManager.h"

static time_t now = 0;
static time_t cach_time = 0;
static int cach_member[512]={0};
static int games[512]={0};
static int g_count = 0;

GetMemberProc::GetMemberProc()
{
	this->name = "GetMemberProc";
}

GetMemberProc::~GetMemberProc()
{

} 

static int GetMemCount(int type, OutputPacket& ReportPkg)
{
	int currNum = 0;
    int baseNum = 0;
	if(now - cach_time >= 30)
	{
		currNum = ServerManager::getInstance()->getCurrUserNum(type,&baseNum);
		currNum += baseNum;
		cach_member[type] = currNum;
		//_LOG_DEBUG_("get from redis\n");
	}
	else
	{	
		currNum = cach_member[type];
		//_LOG_DEBUG_("get from cach\n");
	}

	ReportPkg.WriteShort(type);
    ReportPkg.WriteInt(currNum);
    _LOG_DEBUG_("[Response Data] type=[%d]\n",type);
    _LOG_DEBUG_("[Response Data] currNum=[%d]\n",currNum);

	return currNum+baseNum;
}

int GetMemberProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType();
	short seq = pPacket->GetSeqNum();
	int uid = pPacket->ReadInt();

	now = time(NULL);

	_LOG_INFO_("==>[GetMemberProc] [0x%04x] uid[%d]\n", cmd, uid);
	
	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (client);
	if(now - cach_time >= 30)
	{
		g_count = ServerManager::getInstance()->getGameTypes(games);
	}
	OutputPacket ReportPkg;
	ReportPkg.Begin(cmd,uid);
	ReportPkg.SetSeqNum(seq);
	ReportPkg.WriteShort(0);
	ReportPkg.WriteString("Ok");
	ReportPkg.WriteInt(uid);
	ReportPkg.WriteShort(g_count);
	_LOG_DEBUG_("[Response Data] g_count=[%d]\n",g_count);

	for(int i=0; i<g_count;i++)
	{
		int	type = games[i]; 
		GetMemCount(type,ReportPkg);
	}

	ReportPkg.End();
	this->send(clientHandler, &ReportPkg,false);
	if(now - cach_time >= 30 && g_count!=0)
	{
		cach_time = now;
		//_LOG_DEBUG_("update cach_time=now\n");
	}	
	
	return 0;
}

int GetMemberProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}

