#include <sys/stat.h> 
#include <signal.h> 
#include <cstdlib>
#include "IniFile.h"
#include "Configure.h"
#include "Logger.h"
#include "Version.h"
#include "Protocol.h"
#include "Util.h"

#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif

Configure::Configure() : GameConfig(GAME_TYPE, E_MSG_SOURCE_SERVER, E_CONFIG_ALLOC)
{

}

const char* Configure::ServerName()
{
	return SERVER_NAME;
}


bool Configure::LoadGameConfig()
{
	char key[15];
	sprintf(key,"Alloc_%d",this->m_nLevel);
	numplayer = ReadInt(key,"NumPlayer",4);

	highlimit = ReadInt("Strategy","HighLimit",10000000);
	mul = ReadInt("Strategy","Mul",10);
 
	return true;
}