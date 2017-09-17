#include <map>

#include "ProcessFactory.h"
#include "GameCmd.h"
#include "LoginHallProc.h"
#include "ComingGameProc.h"
#include "LeaveProcess.h"
#include "GameStartProc.h"
#include "SendCardProc.h"
#include "PushGameStartProc.h"
#include "LookCardProc.h"
#include "ThrowCardProc.h"
#include "CallProcess.h"
#include "RaseProcess.h"
#include "AllinProcess.h"
#include "GameOverProc.h"
#include "KickOutPlayerProc.h"
#include "BetTimeOutProc.h"
#include "RobotCardProc.h"
#include "ReloadConf.h"
#include "GetIdleTableProc.h"
#include "HeartProcess.h"

using namespace std;

static std::map<int ,IProcess*>   processMap;

IProcess* ProcessFactory::getProcesser(int cmd)
{
	std::map<int ,IProcess*>* pMap = &processMap;

	std::map<int ,IProcess*>::iterator it;
	it = pMap->find(cmd);
	if(it==pMap->end())
	{
		IProcess* p = createProcesser(cmd);
		(*pMap)[cmd] = p;
		return p;
	}else{
		return it->second;
	}
 }


IProcess* ProcessFactory::createProcesser(int cmd)
{
	IProcess* processer = NULL;
	switch(cmd)
	{
		case ADMIN_GET_IDLE_TABLE:
			processer = new GetIdleTableProc(); break;
		case HALL_USER_LOGIN:
			processer = new LoginHallProc(); break;
		case CLIENT_MSG_LOGINCOMING:
			processer = new ComingGameProc(); break;
		case CLIENT_MSG_LEAVE:
			processer = new LeaveProcess(); break;
		case CLIENT_MSG_START_GAME:
			processer = new GameStartProc(); break;
		case GMSERVER_MSG_SEND_CARD:
			processer = new SendCardProc(); break;
		case GMSERVER_GAME_START:
			processer = new PushGameStartProc(); break;
		case CLIENT_MSG_LOOK_CARD:
			processer = new LookCardProc(); break;
		case CLIENT_MSG_THROW_CARD:
			processer = new ThrowCardProc(); break;
		case CLIENT_MSG_BET_CALL:
			processer = new CallProcess(); break;
		case CLIENT_MSG_BET_RASE:
			processer = new RaseProcess(); break;
		case CLIENT_MSG_ALL_IN:
			processer = new AllinProcess(); break;
		case GMSERVER_MSG_GAMEOVER:
			processer = new GameOverProc(); break;
		case GMSERVER_BET_TIMEOUT:
			processer = new BetTimeOutProc(); break;
		case SERVER_MSG_KICKOUT:
			processer = new KickOutPlayerProc(); break;
		case SERVER_MSG_ROBOT:
			processer = new RobotCardProc(); break;
		case ADMIN_CMD_RELOAD:
			processer = new ReloadConf(); break;
		case USER_HEART_BEAT:
			processer = new HeartProcess(); break;
		default:
			processer = NULL;break;
	}
	return processer;
}

 
