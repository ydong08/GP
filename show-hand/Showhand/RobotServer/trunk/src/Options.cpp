#include <sys/stat.h> 
#include <signal.h> 
#include <stdlib.h>
#include "IniFile.h"
#include "Options.h"
#include "Logger.h"
#include "Version.h"


#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>
#endif

void daemonize()
{
#ifndef WIN32
	pid_t pid, sid;

	/* already a daemon */
	if ( getppid() == 1 ) return;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* At this point we are executing as the child process */

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Redirect standard files to /dev/null */
	freopen( "/dev/null", "r", stdin);
	freopen( "/dev/null", "w", stdout);
	freopen( "/dev/null", "w", stderr);
#endif 
}  

Options::Options()
{
	port = 0;
	basebettime = 0;
	level = 0;
	m_robotId = 0;
}

const char* const short_options = "n:p:i:t:l:u:c:r:a:vdh";  
struct option long_options[] = {  
	 { "number",	1,   NULL,    'n' },
	 { "port",		1,   NULL,    'p'     }, 
	 { "level",          1,   NULL,    'l'     },
     { "daemon",	0,   NULL,    'd'     },
	 { "version",   0,   NULL,    'v'     },
     { "help",		0,   NULL,    'h'     },  
     {      0,     0,     0,     0        },  
}; 

void printHelp()
{
		printf("-%s %-10s %-15s %s\n",				  "n", "--number", "<number>", "robot server number");
		printf("-%s %-10s %-15s %s\n",                "p","--port","","listen port for this server");
		printf("-%s %-10s %-15s %s\n",    			  "l","--level","<level>","robot level");
        printf("-%s %-10s %-15s %s\n",                "d","--daemonize","","run as a daemon");
		printf("-%s %-10s %-15s %s\n",                "v","--version","","print version info");
        printf("-%s %-10s %-15s %s\n",                "h","--help","","print this help and exit");
        printf("\n");
}

int Options::parse_args(int argc, char *argv[]) 
{
	if(argc==1)
	{
		 printHelp();
		 return -1;
	}

	int opt;
	while ((opt = getopt_long (argc, argv, short_options, long_options, NULL)) != -1)  
	{
		switch (opt) 
		{
		case 'n':
			m_robotId = atoi(optarg);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'l':
			level = atoi(optarg);
			break;
		case 'd': 
			{
				signal(SIGINT,  SIG_IGN);
				signal(SIGTERM, SIG_IGN);
				daemonize();
			}
			break;
		case 'h':
			printHelp();
			return -1; 
		case 'f':
			read_conf("../conf/robot.ini");
			printConf();	
			return -1;
		case 'v':       
#ifdef ____DEBUG_VERSION____
			printf("RobotServer server: %s.%s.%s\n",VERSION,SUBVER,"DEBUG_VERSION");
#else                   
			printf("RobotServer server: %s.%s.%s\n",VERSION,SUBVER,"RELEASE_VERSION");
#endif 
			return -1;
		case ':':
			printHelp();
			printf("-%c requires an argument\n",opt);
			return -1;
		default:
			printHelp();
			printf("Parse error.\n");
			return -1;
		}
	}
	if(port == 0 || level == 0)
	{
		printf("Please Input server_port: -p port or -l level\n");
		printHelp();
		return -1;
	}

	if (m_robotId == 0)
	{
		printf("Please Input robot server number, first is 1\n");
		printHelp();
		return -1;
	}

	return 0;
}

int Options::read_conf(const char file[256])
{
	IniFile iniFile(file);
	if(!iniFile.IsOpen())
	{ 
		printf("Open IniFile Error:[%s]\n",file);
		return -1;
	}

	//游戏监听Server
	std::string gms_host =  iniFile.ReadString("GameServer","Address","NULL");
	strcpy( listen_address, gms_host.c_str() );  
		
	//日志
	m_loglevel = iniFile.ReadInt("Log", "LEVEL", 0);
	m_logRemoteIp = iniFile.ReadString("Log", "RemoteIp", "");
	m_logRemotePort = iniFile.ReadInt("Log", "RemotePort", 0);

	monitortime = iniFile.ReadInt("Timer","MonitorTimer",10);
	basebettime = iniFile.ReadInt("Timer","BaseBetTimer",2);

	baseplayCount = iniFile.ReadInt("PlayCount","num",2);

	entertime = iniFile.ReadInt("Coming","entertime",5);

	konwcontrlLevel = iniFile.ReadInt("KonwLevel","level",2);
	baseMoney1 = iniFile.ReadInt("BaseMoney","num1",1000);
	baseMoney2 = iniFile.ReadInt("BaseMoney","num2",30000);
	baseMoney3 = iniFile.ReadInt("BaseMoney","num3",120000);

	randMoney1 = iniFile.ReadInt("RandMoney","num1",5000);
	randMoney2 = iniFile.ReadInt("RandMoney","num2",50000);


	std::string redis_host =  iniFile.ReadString("REDIS","Host","NULL");
    redis_port = iniFile.ReadInt("REDIS","Port",0);
    strcpy( redis_ip, redis_host.c_str() );

	swtichWin1 = iniFile.ReadInt("Swtich","Money1",10000);
	swtichWin2 = iniFile.ReadInt("Swtich","Money2",10000);
	swtichWin3 = iniFile.ReadInt("Swtich","Money3",10000);
	getinredis = iniFile.ReadInt("Swtich","GetInRedis",0);

	baseuid = iniFile.ReadInt("RandUID","basenum",100000);
	randuid = iniFile.ReadInt("RandUID","num",10000);

	//
	std::string server_host1 =  iniFile.ReadString("VRedis1","Host","NULL");
	strcpy( verifyredis_ip1, server_host1.c_str() );
    verifyredis_port1 = iniFile.ReadInt("VRedis1","Port",0);

	//
	std::string server_host2 =  iniFile.ReadString("VRedis2","Host","NULL");
	strcpy( verifyredis_ip2, server_host2.c_str() );
    verifyredis_port2 = iniFile.ReadInt("VRedis2","Port",0);

	std::string conn_hall_host =  iniFile.ReadString("Hall","Host","NULL");
	strcpy( hall_ip, conn_hall_host.c_str() );
    hall_port = iniFile.ReadInt("Hall","Port",0);
	if(hall_port == 0)
	{
		printf("hall_port is null please check your conf\n");
		return -1;
	}
 
	return 0;
	
}


void Options::printConf()
{
	printf("==============SysConfig=================\n");
	printf("LogRemoteIp=[%s]\n", m_logRemoteIp.c_str());
	printf("LogRemotePort=[%d]\n", m_logRemotePort);
	printf("LogLevel=[%d]\n", m_loglevel);
	 
	printf("listen_address=[%s]\n",listen_address);
	printf("port=[%u]\n",port);
	printf("monitortime=[%u]\n",monitortime);

	printf("hall_ip=[%s]\n",hall_ip);
	printf("hall_port=[%u]\n",hall_port);
	printf("baseplayCount=[%u]\n",baseplayCount);
	printf("konwcontrlLevel=[%u]\n",konwcontrlLevel);

	printf("baseMoney1=[%u]\n",baseMoney1);
	printf("baseMoney2=[%u]\n",baseMoney2);
	printf("baseMoney3=[%u]\n",baseMoney3);

	printf("=====================================\n");
}

