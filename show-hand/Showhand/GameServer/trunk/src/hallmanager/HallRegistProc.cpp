#include "HallRegistProc.h"
#include "HallManager.h"
#include "Logger.h"

HallRegistProc::HallRegistProc()
{
	this->name = "HallRegistProc" ;
}

HallRegistProc::~HallRegistProc()
{
}

int HallRegistProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* pPacket,Context* pt)
{
	_NOTUSED(pt);
	int cmd = pPacket->GetCmdType() ;
	//short source = pPacket->GetSource() ;

	int hallid = pPacket->ReadInt();
	
	_LOG_DEBUG_("ParseData hallid=[%d]\n",hallid);
 
	ClientHandler* hallhandler = reinterpret_cast<ClientHandler*> (clientHandler);
	if(hallhandler)
	{
		HallManager::getInstance()->addHallHandler(hallid , hallhandler);
		
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(0); 
		ReportPkg.WriteString("OK"); 
		ReportPkg.End();
		this->send(clientHandler, &ReportPkg);

		_LOG_DEBUG_("RespData error=[%d]\n",0);
		_LOG_DEBUG_("RespData errmsg=[%s]\n","OK");
		return 0;
	}
	else
	{
		OutputPacket ReportPkg;
		ReportPkg.Begin(cmd);
		ReportPkg.WriteShort(-1); 
		ReportPkg.WriteString("CCSocketHandlerHandler can't cast to ClientHandler"); 
		ReportPkg.End();
		this->send(clientHandler, &ReportPkg);

		_LOG_DEBUG_("RespData error=[%d]\n",-1);
		_LOG_DEBUG_("RespData errmsg=[%s]\n","CCSocketHandlerHandler can't cast to HallHandler");
		return -1;
	}

}

int HallRegistProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket ,Context* pt )  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
