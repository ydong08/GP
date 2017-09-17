#include "HallRegistProc.h"
#include "HallManager.h"
#include "ClientHandler.h"
#include "ServerManager.h"
#include "Logger.h"

HallRegistProc::HallRegistProc()
{
	this->name = "HallRegistProc" ;
}

HallRegistProc::~HallRegistProc()
{

}

int HallRegistProc::doRequest(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(pt);
	ClientHandler* clientHandler = reinterpret_cast <ClientHandler*> (socketHandler);
	int cmd = inputPacket->GetCmdType() ;
	int hallid = inputPacket->ReadInt();
	
	_LOG_DEBUG_("ParseData hallid=[%d]\n",hallid);
	if(clientHandler)
	{
		clientHandler->setHandleType(HALLSRV_HANDLE);
		HallManager::getInstance()->addHallHandler(hallid , clientHandler);
		
		OutputPacket ReportPkg;
		ReportPkg.Begin(ALLOC_TRAN_GAMEINFO);
		ServerManager::getInstance()->buildPacketToHall(ReportPkg);
		ReportPkg.End();
		this->send(clientHandler, &ReportPkg, false);
		return 0;
	}
	else
	{
		_LOG_ERROR_("RespData errmsg=[%s]\n","ClientHandler can't cast to HallHandler ClientHandler is NULL");
		return -1;
	}

}

int HallRegistProc::doResponse(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt) 
{
	_NOTUSED(socketHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
