#include <json/json.h>
#include <string>
#include "Configure.h"
#include "GetSvrCfg.h"
#include "GameCmd.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "ProtocolServerId.h"

REGISTER_PROCESS(ADMIN_GET_SYS_CFG, GetSvrCfg)
 

using namespace std;

int GetSvrCfg::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
		short cmd = inputPacket->GetCmdType();

		_LOG_DEBUG_("==>[GetSvrCfg] [0x%04x] \n", cmd);
		char str[64];

    	Json::Value data;

		data["Version"] = string(str); 
		data["ServerID"] = Configure::getInstance()->m_nServerId;
		data["ServerType"] = Configure::getInstance()->m_nLevel;
        data["ServerPort"] = Configure::getInstance()->m_nPort;
        data["ServerPriority"] = Configure::getInstance()->m_nWeight;
        data["ServerHost"] = Configure::getInstance()->m_sListenIp;
		data["AllocIP"] = Configure::getInstance()->alloc_host;
        data["AllocPort"] = Configure::getInstance()->alloc_port;

		data["monitor_time"] = Configure::getInstance()->monitor_time; //监控时间 
        data["keeplive_time"] = Configure::getInstance()->keeplive_time;//存活时间
        data["max_table"] = Configure::getInstance()->max_table; //
        data["max_user"] = Configure::getInstance()->max_user;//
		char time_str[32]={0};
		struct tm* tp= localtime(&Configure::getInstance()->starttime);
		strftime(time_str,32,"%Y/%m/%d %H:%M:%S",tp);
        data["Server_StartTime"] = string(time_str); //
		tp= localtime(&Configure::getInstance()->lasttime);
		strftime(time_str,32,"%Y/%m/%d %H:%M:%S",tp);
        data["Server_LastTime"] = string(time_str);;//
        data["maxcout"] = Room::getInstance()->getMaxCount();//

		
		OutputPacket response;
		response.Begin(ADMIN_GET_SYS_CFG);
		response.WriteString(data.toStyledString());
		response.End();
		this->send(clientHandler, &response);
		return  0;
}

int GetSvrCfg::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{		
	return 0;	
}  


int GetSvrCfg::sendErrorMsg(CDLSocketHandler* clientHandler, short errcode, char* errmsg) 
{
		OutputPacket response;
		response.Begin(ADMIN_GET_SYS_CFG);
		response.WriteShort(errcode);
		response.WriteString(errmsg);
		response.End();
		_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", errcode);
		_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", errmsg);
		this->send(clientHandler, &response);
		return 0;
}

 
