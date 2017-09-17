#include <map>

#include "ProcessFactory.h"
#include "GameCmd.h"
#include "HeartBeatProc.h"
#include "HallRegistProc.h"
#include "HallHeartBeatProc.h"
#include "LoginComingProc.h"
#include "StartGameProc.h"
#include "BetCallProc.h"
#include "BetRaseProc.h"
#include "AllinProc.h"
#include "LookCardProc.h"
#include "ThrowCardProc.h"
#include "LeaveProcess.h"
#include "LogoutProcess.h"
#include "GetTableDetailProc.h"
#include "GetFreePlayer.h"
#include "Reload.h"
#include "GetRoomInfoProcess.h"
#include "GetSvrCfg.h"
#include "GetAllUserStatus.h"
#include "EnterPrivateRoomProc.h"
#include "GetIdleTableProc.h"
#include "UpdateMoneyProc.h"
#include "ChatProcess.h"
#include "ProtocolClientId.h"


static std::map<int ,IProcess*>  * processMap = new  std::map<int ,IProcess*> ();

IProcess* ProcessFactory::getProcesser(int cmd)
{
	std::map<int ,IProcess*>* pMap = processMap;

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
		case HALL_REGIST_INFO:
			processer = new HallRegistProc(); break;
		case HALL_HEART_BEAT:
			processer = new HallHeartBeatProc(); break;
		case USER_HEART_BEAT:
			processer = new HeartBeatProc(); break;
		case CLIENT_MSG_LOGINCOMING:
			processer = new LoginComingProc();break;
		case CLIENT_MSG_START_GAME:
			processer = new StartGameProc();break;
		case CLIENT_MSG_BET_CALL:
			processer = new BetCallProc();break;
		case CLIENT_MSG_BET_RASE:
			processer = new BetRaseProc();break;
		case CLIENT_MSG_ALL_IN:
			processer = new AllinProc();break;
		case CLIENT_MSG_LOOK_CARD:
			processer = new LookCardProc();break;
		case CLIENT_MSG_THROW_CARD:
			processer = new ThrowCardProc();break;
		case CLIENT_MSG_CHAT:
			processer = new ChatProcess();break;
		case CLIENT_MSG_LEAVE:
			processer = new LeaveProcess();break;
		case CLIENT_MSG_LOGOUT:
			processer = new LogoutProcess();break;
		case CLIENT_MSG_TABLEDET:
			processer = new GetTableDetailProc();break;
		case CLIENT_ENTER_PRIVATE:
			processer = new EnterPrivateRoomProc();break;
//=====================系统管理接口============================//
		case ADMIN_MSG_ROOMINFO: 
			processer = new GetRoomInfoProcess(); break;
		case ADMIN_GET_FREE_PLAYER:
			processer = new GetFreePlayer(); break;
		case ADMIN_GET_SYS_CFG:
			processer = new GetSvrCfg(); break;
		case ADMIN_RELOAD:
			processer = new Reload(); break;
		case ADMIN_GET_ALL_PLAYER:
			processer = new GetAllUserStatus(); break;
		case ADMIN_GET_IDLE_TABLE:
			processer = new GetIdleTableProc(); break;

		case MONEYSERVER_UPDATE_USERCOIN:
			processer = new UpdateMoneyProc(); break;

		
		default:
			processer = NULL;break;
	}
	return processer;
}

 
