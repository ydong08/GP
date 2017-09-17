#include <json/json.h>
#include <string>
#include "Configure.h"
#include "GetSvrCfg.h"
#include "../GameCmd.h"
#include "../model/Room.h"
#include "../Version.h"
#include "Logger.h"
 

using namespace std;

int GetSvrCfg::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
		short cmd = inputPacket->GetCmdType();

		_LOG_DEBUG_("==>[GetSvrCfg] [0x%04x] \n", cmd);
		char str[64];

    	Json::Value data;

#ifdef ____DEBUG_VERSION____
		sprintf(str,"%s.%s.%s", VERSION, SUBVER, "DEBUG_VERSION");
#else
		sprintf(str,"%s.%s.%s", VERSION, SUBVER, "RELEASE_VERSION");
#endif
		data["Version"] = string(str); 
		data["ServerID"] = Configure::getInstance()->server_id;
		data["ServerType"] = Configure::getInstance()->level;
        data["ServerPort"] = Configure::getInstance()->port;
        data["ServerPriority"] = Configure::getInstance()->server_priority;
        data["ServerHost"] = Configure::getInstance()->report_ip;
        data["LogFile"] = Configure::getInstance()->logfile;
        data["LogLevel"] = Configure::getInstance()->loglevel;
		data["AllocIP"] = Configure::getInstance()->alloc_ip;
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
        data["IsLogReport"] = Configure::getInstance()->isLogReport;//
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

 
