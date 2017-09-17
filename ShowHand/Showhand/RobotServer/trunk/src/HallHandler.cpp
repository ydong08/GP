#include "GameCmd.h"
#include "CDLReactor.h"
#include "Despatch.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"

using namespace std;

static InputPacket pPacket;
//=================HallHandler=========================
HallHandler::HallHandler()
{
	uid = -1;
}

HallHandler::~HallHandler( )
{
	uid = -1;
}

int HallHandler::OnConnected()
{
	LOGGER(E_LOG_INFO) << "Connect netfd = " << netfd << " Success";
	return 0;
}

int HallHandler::OnClose()
{
	LOGGER(E_LOG_INFO) << "Connect Close netfd = " << netfd << " Success";
	return 0;
}

int HallHandler::OnPacketComplete(const char* data, int len)
{
	pPacket.Copy(data,len);
    short cmd = pPacket.GetCmdType() ;

	LOGGER(E_LOG_DEBUG) << "BackNode Recv Packet Cmd = " << cmd;
    return Despatch::getInstance()->doBackResponse(this,&pPacket);
}

