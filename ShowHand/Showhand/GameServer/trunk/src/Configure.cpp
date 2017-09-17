#include <sys/stat.h> 
#include <signal.h> 
#include "Version.h"
#include "IniFile.h"
#include "Configure.h"
#include "Logger.h"


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

Configure::Configure()
{
	level = 0;
	server_id = 0;
	server_priority = 0 ;
	max_table = 0;
	max_user = 0;
	report_ip[0]='\0';

	starttime = time(NULL);
	lasttime = starttime;
	isLogReport = 0;
}

const char* const short_options = "p:s:t:l:u:c:r:vdh";  
struct option long_options[] = {  
	 { "port",		1,   NULL,    'p'     },  
	 { "sid",		1,   NULL,    's'     },  
	 { "level",          1,   NULL,    'l'     },
	 { "priority",      1,   NULL,    'r'     },
	 { "maxuser",       1,   NULL,    'u'     },
	 { "maxtab",        1,   NULL,    'c'     },
	 { "daemon",	0,   NULL,    'd'     },  
	 { "version",	0,   NULL,    'v'     },  
	 { "help",		0,   NULL,    'h'     },  
	 { "config",      0,   NULL,    'f'     },
	 {      0,     0,     0,     0},  
}; 

void printHelp()
{
#ifdef ____DEBUG_VERSION____
			printf("showhand server: %s.%s.%s\n",VERSION,SUBVER,"DEBUG_VERSION");
#else
			printf("showhand server: %s.%s.%s\n",VERSION,SUBVER,"RELEASE_VERSION");
#endif
		printf("-%s %-10s %-15s %s\n",                "p","--port","<port>","tcp port number to listen on");
		printf("-%s %-10s %-15s %s\n",    			  "l","--level","<level>","server level for player 1- 50 coin, 2- 500 coin, 3- 5000 coin , 4- 50000 coin");
		printf("-%s %-10s %-15s %s\n",                "s","--svid","<server id>","server id of this progress");
		printf("-%s %-10s %-15s %s\n",                "r","--priority","<server priority>","priority  for this game progress");
		printf("-%s %-10s %-15s %s\n",                "u","--maxuser","<max user>","max online user count");
		printf("-%s %-10s %-15s %s\n",                "c","--maxtab","<max table>","max table count");
		printf("-%s %-10s %-15s %s\n",                "r","--reportip","<report ip>","");
		printf("-%s %-10s %-15s %s\n",                "d","--daemonize","","run as a daemon");
		printf("-%s %-10s %-15s %s\n",                "v","--version","","print version info");
		printf("-%s %-10s %-15s %s\n",                "h","--help","","print this help and exit");
		printf("-%s %-10s %-15s %s\n",                "f","--config","","print config info");
		printf("\n");
}

int Configure::parse_args(int argc, char *argv[]) 
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
		case 'p':
			port = atoi(optarg);
			break;
		case 'l':
			level = atoi(optarg);
			break;
		case 's':		//server Id
			server_id = atoi(optarg);
			break;
		case 'r':
			server_priority = atoi(optarg);
			break;
		case 'u':
			max_user = atoi(optarg);
			break;
		case 'c':
			max_table = atoi(optarg);
			break;
		case 'd': 
			{ 
				signal(SIGINT,  SIG_IGN);
				signal(SIGTERM, SIG_IGN);
				daemonize();
			}
			break;
		case 'v':
#ifdef ____DEBUG_VERSION____
			printf("showhand server: %s.%s.%s\n",VERSION,SUBVER,"DEBUG_VERSION");
#else
			printf("showhand server: %s.%s.%s\n",VERSION,SUBVER,"RELEASE_VERSION");
#endif
			return -1;
		case 'h':
			printHelp();
			return -1; 
		case 'f':
			read_conf("../conf/showhand.ini");
			printConf();	
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
   
   if(port==0)
	{
		printf("Please Input server_port: -p port\n");
		printHelp();
		return -1;
   }
	 
	 if(server_id==0)
	{
		 printf("Please Input server_id: -s svid\n");
		printHelp();
		return -1;
   }

	if(level==0)
	{
		 printf("Please Input level: -l level\n");
		printHelp();
		return -1;
   }

	return 0;
}

int Configure::read_conf(const char file[256])
{
	IniFile iniFile(file);
	if(!iniFile.IsOpen())
	{ 
		printf("Open IniFile Error:[%s]\n",file);
		return -1;
	}
		
	//日志
	m_loglevel = iniFile.ReadInt("Log", "LEVEL", 0);
	m_logRemoteIp = iniFile.ReadString("Log", "RemoteIp", "");
	m_logRemotePort = iniFile.ReadInt("Log", "RemotePort", 0);
 
	//最大棋桌 最大人数
	if(max_table==0)
		max_table = iniFile.ReadInt("Room" ,"MaxTable", 100);
	if(max_user==0)
		max_user  = iniFile.ReadInt("Room" ,"MaxUser", 500);

	monitor_time = iniFile.ReadInt("Room" ,"MonitorTime", 30);
	keeplive_time  = iniFile.ReadInt("Room" ,"KeepliveTime", 100);
	wincoin1 = iniFile.ReadInt("Room", "TrumptCoin1", 50000);
	wincoin2 = iniFile.ReadInt("Room", "TrumptCoin2", 10000);
	wincoin3 = iniFile.ReadInt("Room", "TrumptCoin3", 5000);
	wincoin4 = iniFile.ReadInt("Room", "TrumptCoin4", 2000);
	loserate = iniFile.ReadInt("Room" ,"LoseRate", 80);
	contrllevel = iniFile.ReadInt("Room" ,"ContrlLevel", 2);

	std::string report = iniFile.ReadString("Report", "IP", "");
	strcpy( report_ip, report.c_str() );

	char key[15];
	sprintf(key,"Alloc_%d",this->level);	
	//匹配服务器
	std::string alloc_host =  iniFile.ReadString(key,"Host","NULL");
	strcpy( alloc_ip, alloc_host.c_str() );
    alloc_port = iniFile.ReadInt(key,"Port",0);
	numplayer = iniFile.ReadInt(key,"NumPlayer",4);
	rewardcoin = iniFile.ReadInt(key,"RewardCoin",1000000);
	rewardroll = iniFile.ReadInt(key,"RewardRoll",1000000);
	rewardRate = iniFile.ReadInt(key,"RewardRate",80);

	//数据库操作服务器
	std::string back_mysql_host =  iniFile.ReadString("MySqlServer","Host","NULL");
	strcpy( mysql_ip, back_mysql_host.c_str() );
    mysql_port = iniFile.ReadInt("MySqlServer","Port",0);

	//经验服务器
	std::string back_round_host =  iniFile.ReadString("RoundServer","Host","NULL");
	strcpy( round_ip, back_round_host.c_str() );
    round_port = iniFile.ReadInt("RoundServer","Port",0);

	//日志服务器
	std::string log_server_host =  iniFile.ReadString("LogServer","Host","NULL");
	strcpy( log_server_ip, log_server_host.c_str() );
    log_server_port = iniFile.ReadInt("LogServer","Port",0);
	isLogReport =  iniFile.ReadInt("LogServer" ,"IsLogReport", 0);
	//redis ip port
	std::string redis_host =  iniFile.ReadString("REDIS","Host","NULL");
    redis_port = iniFile.ReadInt("REDIS","Port",0);
    strcpy( redis_ip, redis_host.c_str() );

	//redis ip port
	std::string redis_thost =  iniFile.ReadString("REDIS","THost","NULL");
    redis_tport = iniFile.ReadInt("REDIS","TPort",0);
    strcpy( redis_tip, redis_thost.c_str() );

	//redis ip port
	std::string redis_nhost =  iniFile.ReadString("REDIS","NHost","NULL");
    redis_nport = iniFile.ReadInt("REDIS","NPort",0);
    strcpy( redis_nip, redis_nhost.c_str() );

	std::string redist_host =  iniFile.ReadString("REDIS","TaskHost","NULL");
    taskredis_port = iniFile.ReadInt("REDIS","TaskPort",0);
    strcpy( taskredis_ip, redist_host.c_str() );

	//服务器
	std::string  server_host =  iniFile.ReadString("GameServer","Host","0.0.0.0");
	strcpy( listen_address, server_host.c_str() );

	if(alloc_port == 0 || mysql_port == 0 ||log_server_port == 0 || redis_port == 0)
	{
		printf("allocinfo alloc_port is null is error or mysql_port or log_server is null or redis_port is null level:%d\n", this->level);
		return -1;
	}

	//屏蔽词服务器
	std::string word_server_host =  iniFile.ReadString("WordServer","Host","NULL");
	strcpy( word_server_ip, word_server_host.c_str() );
    word_server_port = iniFile.ReadInt("WordServer","Port",0);

	//UDP服务器
	std::string udp_server_host =  iniFile.ReadString("UDPServer","Host","NULL");
	strcpy( udp_ip, udp_server_host.c_str() );
    udp_port = iniFile.ReadInt("UDPServer","Port",0);

	maxmulone = iniFile.ReadInt("MaxMul","MulOneRound",4);
	maxmultwo = iniFile.ReadInt("MaxMul","TwoOneRound",5);
	betcointime = iniFile.ReadInt("Timer","BetCoinTime",20);
	tablestarttime = iniFile.ReadInt("Timer","TableStartTime",20);
	kicktime = iniFile.ReadInt("Timer","KickTime",10);

	timeoutCount = iniFile.ReadInt("TimeOutCount","num",2);
	fraction = iniFile.ReadInt("Fraction","num",100);

	esayTaskCount = iniFile.ReadInt("EsayTask","PlayCount",30);
	esayRandNum = iniFile.ReadInt("EsayTask","RandNum",60);
	esayTaskRand = iniFile.ReadInt("EsayTask","TaskRand",50);
	curBeforeCount = iniFile.ReadInt("EsayTask","curBeforeCount",10);
	esayTaskProbability = iniFile.ReadInt("EsayTask","esayTaskProbability",80);

	getIngotNoti1 = iniFile.ReadInt("Notify","Num1",10);
	getIngotNoti2 = iniFile.ReadInt("Notify","Num2",10);
	getIngotNoti3 = iniFile.ReadInt("Notify","Num3",10);

	robotTabNum1 = iniFile.ReadInt("RobotTab","Num1",10);
	robotTabNum2 = iniFile.ReadInt("RobotTab","Num2",10);
	robotTabNum3 = iniFile.ReadInt("RobotTab","Num3",2);
	
	for(int i = 0; i < 10; ++i)
	{
		char szhost[16];
		char szport[16];
		sprintf(szhost,"Host_%d",i);
		sprintf(szport,"Port_%d",i);
		std::string temp_server_host =  iniFile.ReadString("CheckRedis",szhost,"NULL");
		strcpy( CheckInGame[i].host, temp_server_host.c_str() );
		CheckInGame[i].port = iniFile.ReadInt("CheckRedis",szport,0);
	}

    std::string offline_redis_host = iniFile.ReadString("REDIS", "OfflineHost", "NULL");
    offline_redis_port = iniFile.ReadInt("REDIS", "OfflineRedisPort", 6380);
    strcpy(offline_redis_ip, offline_redis_host.c_str());

	//运维用服务器
	std::string operation_redis_host = iniFile.ReadString("REDIS", "OperationHost", "NULL");
	operation_redis_port = iniFile.ReadInt("REDIS", "OperationPort", 0);
	strcpy(operation_redis_ip, operation_redis_host.c_str());
	return 0;
	
}


void Configure::printConf()
{
	printf("==============SysConfig=================\n");
	printf("LogRemoteIp=[%s]\n", m_logRemoteIp.c_str());
	printf("LogRemotePort=[%d]\n", m_logRemotePort);
	printf("LogLevel=[%d]\n", m_loglevel);
	
	printf("server_id=[%d]\n",server_id);
	printf("server_level=[%d]\n",level);
	printf("listen_address=[%s]\n",listen_address);
	printf("server_port=[%u]\n",port);
	printf("server_priority=[%d]\n",server_priority);
	printf("report_ip=[%s]\n",report_ip);

	printf("redis_host=[%s]\n",redis_ip);
	printf("redis_port=[%u]\n",redis_port);

	printf("redis_thost=[%s]\n",redis_tip);
	printf("redis_tport=[%u]\n",redis_tport);

	printf("redis_nhost=[%s]\n",redis_nip);
	printf("redis_nport=[%u]\n",redis_nport);

	printf("MaxTable=[%d]\n",max_table);
	printf("MaxUser=[%d]\n",max_user);

	printf("monitor_time=[%u]\n",monitor_time);
	printf("keeplive_time=[%u]\n",keeplive_time);

	printf("alloc_ip=[%s]\n",alloc_ip);
	printf("alloc_port=[%u]\n",alloc_port);
	printf("numplayer=[%u]\n",numplayer);

	printf("udp_ip=[%s]\n",udp_ip);
	printf("udp_port=[%u]\n",udp_port);

	printf("maxmulone=[%d]\n",maxmulone);
	printf("maxmultwo=[%d]\n",maxmultwo);
	printf("betcointime=[%d]\n",betcointime);
	printf("tablestarttime=[%d]\n",tablestarttime);
	printf("timeoutCount=[%d]\n",timeoutCount);
	printf("kicktime=[%d]\n",kicktime);
	printf("fraction=[%d]\n",fraction);
	printf("esayTaskCount=[%d]\n",esayTaskCount);
	printf("getIngotNoti1=[%d]\n",getIngotNoti1);
	printf("getIngotNoti2=[%d]\n",getIngotNoti2);
	printf("getIngotNoti3=[%d]\n",getIngotNoti3);

	
	printf("=====================================\n");
}

