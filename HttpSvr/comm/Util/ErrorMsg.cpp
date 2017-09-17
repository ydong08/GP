#include <map>
#include <string>
#include <cstdio>

#include "ErrorMsg.h"
#include "IniFile.h"

static ErrorMsg* instance = NULL;
static std::map<int, std::string> err_code_msg ;
static const char* no_err_msg_find="error msg"; 
 
ErrorMsg::ErrorMsg()
{
}

ErrorMsg::~ErrorMsg()
{

}

ErrorMsg* ErrorMsg::getInstance()
{
	if(instance==NULL)
	{
		instance = new ErrorMsg();
		instance->init();
	}
	return instance;
}

const char* ErrorMsg::getErrMsg(int code)
{
	 std::map<int , std::string>::iterator it = err_code_msg.find(code);
	 if(it == err_code_msg.end())
		 return no_err_msg_find;
	 else
		 return it->second.c_str();
}

void ErrorMsg::init(const char* file)
{
	IniFile iniFile(file);
	if(!iniFile.IsOpen())
	{
		printf("Open IniFile Error:[%s]\n",file);
		return ;
	}
	//日志
	err_code_msg[0] = iniFile.ReadString("Error", "0", "");
	err_code_msg[-1] = iniFile.ReadString("Error","-1","") ;
	err_code_msg[-2] = iniFile.ReadString("Error","-2","");
	err_code_msg[-3] = iniFile.ReadString("Error","-3","");
	err_code_msg[-4] = iniFile.ReadString("Error","-4","");
	err_code_msg[-5] = iniFile.ReadString("Error","-5","");
	err_code_msg[-6] = iniFile.ReadString("Error","-6","");
	err_code_msg[-7] = iniFile.ReadString("Error","-7","");
	err_code_msg[-8] = iniFile.ReadString("Error","-8","");
	err_code_msg[-9] = iniFile.ReadString("Error","-9","");
	err_code_msg[-10] = iniFile.ReadString("Error","-10","");
	err_code_msg[-11] = iniFile.ReadString("Error","-11","");
	err_code_msg[-12] = iniFile.ReadString("Error","-12","");
	err_code_msg[-13] = iniFile.ReadString("Error","-13","");
	err_code_msg[-14] = iniFile.ReadString("Error","-14","");
	err_code_msg[-15] = iniFile.ReadString("Error","-15","");
	err_code_msg[-16] = iniFile.ReadString("Error","-16","");
	err_code_msg[-17] = iniFile.ReadString("Error","-17","");
	err_code_msg[-18] = iniFile.ReadString("Error","-18","");
	err_code_msg[-19] = iniFile.ReadString("Error","-19","");
	err_code_msg[-20] = iniFile.ReadString("Error","-20","");
	err_code_msg[-21] = iniFile.ReadString("Error","-21","");
	err_code_msg[-22] = iniFile.ReadString("Error","-22","");
	err_code_msg[-23] = iniFile.ReadString("Error","-23","");
	err_code_msg[-24] = iniFile.ReadString("Error","-24","");
	err_code_msg[-25] = iniFile.ReadString("Error","-25","");
	err_code_msg[-26] = iniFile.ReadString("Error","-26","");
	err_code_msg[-27] = iniFile.ReadString("Error","-27","");
	err_code_msg[-28] = iniFile.ReadString("Error","-28","");
	err_code_msg[-29] = iniFile.ReadString("Error","-29","");
	err_code_msg[-30] = iniFile.ReadString("Error","-30","");
	err_code_msg[-31] = iniFile.ReadString("Error", "-31", "");
	err_code_msg[-32] = iniFile.ReadString("Error", "-32", "");
	err_code_msg[-33] = iniFile.ReadString("Error", "-33", "");
	err_code_msg[-34] = iniFile.ReadString("Error", "-34", "");
	err_code_msg[-35] = iniFile.ReadString("Error", "-35", "");
	err_code_msg[-36] = iniFile.ReadString("Error", "-36", "");
	err_code_msg[-37] = iniFile.ReadString("Error", "-37", "");
	err_code_msg[-38] = iniFile.ReadString("Error", "-38", "");
	err_code_msg[-39] = iniFile.ReadString("Error", "-39", "");

	err_code_msg[1] = iniFile.ReadString("Error","1","") ;
	err_code_msg[2] = iniFile.ReadString("Error","2","");
	err_code_msg[3] = iniFile.ReadString("Error","3","");
	err_code_msg[4] = iniFile.ReadString("Error","4","");
	err_code_msg[5] = iniFile.ReadString("Error","5","");
	err_code_msg[6] = iniFile.ReadString("Error","6","");
	err_code_msg[7] = iniFile.ReadString("Error","7","");
	err_code_msg[8] = iniFile.ReadString("Error","8","");
	err_code_msg[9] = iniFile.ReadString("Error","9","");
	err_code_msg[10] = iniFile.ReadString("Error","10","");
}


void ErrorMsg::initFromBuf(const char *pErrMsg)
{
	IniFile iniFile;
	if (!iniFile.ParseBuffer(pErrMsg))
	{
		printf("Parse Ini Buffer Error\n");
		return;
	}
	//日志
	err_code_msg[0] = iniFile.ReadString("Error", "0", "");
	err_code_msg[-1] = iniFile.ReadString("Error", "-1", "");
	err_code_msg[-2] = iniFile.ReadString("Error", "-2", "");
	err_code_msg[-3] = iniFile.ReadString("Error", "-3", "");
	err_code_msg[-4] = iniFile.ReadString("Error", "-4", "");
	err_code_msg[-5] = iniFile.ReadString("Error", "-5", "");
	err_code_msg[-6] = iniFile.ReadString("Error", "-6", "");
	err_code_msg[-7] = iniFile.ReadString("Error", "-7", "");
	err_code_msg[-8] = iniFile.ReadString("Error", "-8", "");
	err_code_msg[-9] = iniFile.ReadString("Error", "-9", "");
	err_code_msg[-10] = iniFile.ReadString("Error", "-10", "");
	err_code_msg[-11] = iniFile.ReadString("Error", "-11", "");
	err_code_msg[-12] = iniFile.ReadString("Error", "-12", "");
	err_code_msg[-13] = iniFile.ReadString("Error", "-13", "");
	err_code_msg[-14] = iniFile.ReadString("Error", "-14", "");
	err_code_msg[-15] = iniFile.ReadString("Error", "-15", "");
	err_code_msg[-16] = iniFile.ReadString("Error", "-16", "");
	err_code_msg[-17] = iniFile.ReadString("Error", "-17", "");
	err_code_msg[-18] = iniFile.ReadString("Error", "-18", "");
	err_code_msg[-19] = iniFile.ReadString("Error", "-19", "");
	err_code_msg[-20] = iniFile.ReadString("Error", "-20", "");
	err_code_msg[-21] = iniFile.ReadString("Error", "-21", "");
	err_code_msg[-22] = iniFile.ReadString("Error", "-22", "");
	err_code_msg[-23] = iniFile.ReadString("Error", "-23", "");
	err_code_msg[-24] = iniFile.ReadString("Error", "-24", "");
	err_code_msg[-25] = iniFile.ReadString("Error", "-25", "");
	err_code_msg[-26] = iniFile.ReadString("Error", "-26", "");
	err_code_msg[-27] = iniFile.ReadString("Error", "-27", "");
	err_code_msg[-28] = iniFile.ReadString("Error", "-28", "");
	err_code_msg[-29] = iniFile.ReadString("Error", "-29", "");
	err_code_msg[-30] = iniFile.ReadString("Error", "-30", "");
	err_code_msg[-31] = iniFile.ReadString("Error", "-31", "");
	err_code_msg[-32] = iniFile.ReadString("Error", "-32", "");
	err_code_msg[-33] = iniFile.ReadString("Error", "-33", "");
	err_code_msg[-34] = iniFile.ReadString("Error", "-34", "");
	err_code_msg[-35] = iniFile.ReadString("Error", "-35", "");
	err_code_msg[-36] = iniFile.ReadString("Error", "-36", "");
	err_code_msg[-37] = iniFile.ReadString("Error", "-37", "");
	err_code_msg[-38] = iniFile.ReadString("Error", "-38", "");
	err_code_msg[-39] = iniFile.ReadString("Error", "-39", "");

	err_code_msg[1] = iniFile.ReadString("Error", "1", "");
	err_code_msg[2] = iniFile.ReadString("Error", "2", "");
	err_code_msg[3] = iniFile.ReadString("Error", "3", "");
	err_code_msg[4] = iniFile.ReadString("Error", "4", "");
	err_code_msg[5] = iniFile.ReadString("Error", "5", "");
	err_code_msg[6] = iniFile.ReadString("Error", "6", "");
	err_code_msg[7] = iniFile.ReadString("Error", "7", "");
	err_code_msg[8] = iniFile.ReadString("Error", "8", "");
	err_code_msg[9] = iniFile.ReadString("Error", "9", "");
	err_code_msg[10] = iniFile.ReadString("Error", "10", "");
}