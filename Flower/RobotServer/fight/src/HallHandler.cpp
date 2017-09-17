#include "GameCmd.h"
#include "CDLReactor.h"
#include "Despatch.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "PlayerManager.h"

using namespace std;

static InputPacket pPacket;
//=================HallHandler=========================
HallHandler::HallHandler()
{
	uid = -1;
}

HallHandler::~HallHandler( )
{
	//this->_decode = NULL;
	uid = -1;
}

int HallHandler::OnConnected()
{
	LOGGER(E_LOG_INFO) << "Success Connect netfd=" << netfd;
	return 0;
}

int HallHandler::OnClose()
{
	LOGGER(E_LOG_INFO) << "Success Connect Close fd=" << netfd;
	PlayerManager::getInstance().delPlayer(this->uid, true);
	return 0;
}

int HallHandler::OnPacketComplete(const char* data, int len)
{
	pPacket.Copy(data,len);
    return Despatch::getInstance().doBackResponse(this, &pPacket);
}

