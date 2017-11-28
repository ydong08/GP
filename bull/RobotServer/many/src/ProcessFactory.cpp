#include "GameCmd.h"
#include "ProcessFactory.h"
#include "IProcess.h"
#include "LoginProcess.h"
#include "LogoutProcess.h"
#include "LoginComingProc.h"
#include "UpdateTabStatusProc.h"
#include "GameOverProc.h"
#include "GetDetailProcess.h"
#include "BetCoinProcess.h"
#include "ApplyBankerProc.h"
#include "RotateProcess.h"
#include "NoteLeaveListProc.h"
#include "HeartProcess.h"
#include "GetPlayInfo.h"
#include "ServerClose.h"
#include "ReloadConf.h"
#include "KickOutPlayerProc.h"
#include <map>

using namespace std;

static std::map<int ,IProcess*>   processMap  ;

IProcess* ProcessFactory::getProcesser(int cmd)
{
	std::map<int ,IProcess*>::iterator it;
	it = processMap.find(cmd);
	if(it==processMap.end())
	{
		IProcess* p = createProcesser(cmd);
        if(p)
		    processMap[cmd] = p;
		return p;
	}else{
		return it->second;
	}
}

IProcess* ProcessFactory::createProcesser(int cmd)
{
 IProcess* process = NULL;
    switch(cmd)
    {
		  case USER_HEART_BEAT:    process = new HeartProcess();  break;
		  case HALL_USER_LOGIN:    process = new LoginProcess();  break;
		  case CLIENT_MSG_LEAVE:   process = new LogoutProcess();break;
		  case CLIENT_MSG_LOGINCOMING: process = new LoginComingProc(); break;
		  case GMSERVER_MSG_GAMEOVER: process = new GameOverProc(); break;
		  case CLIENT_MSG_TABLEDET: process = new GetDetailProcess(); break;
		  case CLIENT_MSG_BET_COIN: process = new BetCoinProcess(); break;
		  case GMSERVER_MSG_TABSTATUS: process = new UpdateTabStatusProc(); break;
		  case CLIENT_MSG_APPLYBANKER: process = new ApplyBankerProc(); break;
		  case GMSERVER_MSG_ROTATEBANKE: process = new RotateProcess(); break;
		  case GMSERVER_MSG_NOTELEAVE: process = new NoteLeaveListProc(); break;
		  case ADMIN_GET_PLAY_INFO: process = new GetPlayInfo(); break;
		  case ADMIN_RELOAD_CONF: process = new ReloadConf(); break;
		  case SERVER_MSG_KICKOUT: process = new KickOutPlayerProc(); break;

		  case SERVER_GAMESERVER_CLOSED: process = new ServerClose(); break;
    }
    return process ;
}
