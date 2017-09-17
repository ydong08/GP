#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include "GameCmd.h"
#include "IProcess.h"
#include "BackConnect.h"
#include "ServerManager.h"
//#include "RedisConnector.h"
#include "Logger.h"
#include "RedisAccess.h"
#include "StrFunc.h"
#include "hiredis.h"

using namespace std;

static GameServer servers[1024];
static int svr_count = 0;

extern RedisAccess *RedisConnector;

static ServerManager* instance = NULL;

static vector<string> split(const string& src,const string& tok, bool trim , string null_subst );

ServerManager* ServerManager::getInstance()
{
	if(instance==NULL)
	{
		instance = new ServerManager();
	}
	return instance;
}

bool ServerManager::initRedis(const char * ip, int port)
{
	std::string addr = StrFormatA("%s:%d", ip, port);
	return m_redis.connect(addr.c_str());
}

//冒泡排序  0 DESC 1 ASC
void BubbleSort(GameServer* serverlist[], int size , int fieldName, int flag=0) ;
void orderServerBySvid(GameServer* serverlist[], int size);
void orderServerByPriority(GameServer* serverlist[], int size );
void orderServerByType(GameServer* serverlist[], int size );
void orderServerByUser(GameServer* serverlist[], int size );
 
 
int ServerManager::addServer(int id,  GameServer* server)
{
	if(server==NULL)
		return -1;
	_svidsMap[id] = server;
	redisReply* reply = RedisConnector->CommandV("HMSET Server:%d  svid %d  "
																				    "ip %s  " 
																					"port %d  "
																					"level %d  "
																					"status %d  "
																					"priority %d  "
																					"maxUserCount %d  "
																					"maxTabCount %d  "
																					"currUserCount %d  "
																					"currTabCount %d  "
																					"lasttime %d  "
																					"gameid %d  ",
																					server->svid ,  server->svid , 
																					server->ip.c_str(), server->port,
																					server->level, server->status,
																					server->priority, server->maxUserCount,
																					server->maxTabCount, server->currUserCount,	
																					0, server->lasttime,
																					server->gameID);
    if(reply)
	{
		freeReplyObject(reply);
		//return 0;
		return updateCurrUserNum(server);
	}
	else
		return -1;
	return 0;
}

int ServerManager::updateServerMember(int id, GameServer* server)
{
	if(server==NULL)
		return -1;

	map<int , GameServer*>::iterator it = _svidsMap.find(id);
	if(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		pserver->maxUserCount = server->maxUserCount;
		pserver->maxTabCount = server->maxTabCount;
		pserver->currUserCount = server->currUserCount; 
		pserver->currTabCount = server->currTabCount;
		pserver->lasttime = server->lasttime;
		pserver->IPhoneUser = server->IPhoneUser;
		pserver->AndroidUser = server->AndroidUser;	
		pserver->IPadUser = server->IPadUser;
		pserver->RobotUser = server->RobotUser;
		pserver->gameID = server->gameID;
	}
	else
		_LOG_WARN_("[ServerManager:updateServerMember] Can't Find Server in Process Memory For id[%d]\n", id);

	redisReply* reply = RedisConnector->CommandV("HMSET Server:%d     svid %d  "
																		"maxUserCount %d  " 
																		"maxTabCount %d  "
																		"currUserCount %d  "
																		"currTabCount %d  "
																		"lasttime %d  "
																		"IPhoneUser %d  "
																		"AndroidUser %d  "
																		"IPadUser %d  "
																		"RobotUser %d  "
																		"gameid %d  ",																			  
																		server->svid , server->svid , 
																		server->maxUserCount, server->maxTabCount,
																		server->currUserCount, 0,
																		server->lasttime,
																		server->IPhoneUser, server->AndroidUser,	
																		server->IPadUser, server->RobotUser,
																		server->gameID);
    if(reply)
	{
		freeReplyObject(reply);
		//return 0;
		return updateCurrUserNum(server); //add by maoshuoqiong 20160818
	}
	else
		return -1;

}

int ServerManager::delServer(int id)
{
	map<int , GameServer*>::iterator it = _svidsMap.find(id);
	if(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		delete pserver;
		_svidsMap.erase(id);
	}
	else
		_LOG_WARN_("[ServerManager:delServer] Can't Find Server in Process Memory For id[%d]\n", id);

	redisReply* reply = RedisConnector->CommandV("DEL Server:%d ", id);
	if(reply)
	{
	    freeReplyObject(reply);
	    return 0;
	}
	else
	    return -1;
}


void ServerManager::buildPacketToHall(OutputPacket& ReportPkg)
{
	ReportPkg.WriteShort((short)_svidsMap.size());
	//printf("_svidsMap.size():%d\n", _svidsMap.size());
	map<int , GameServer*>::iterator it = _svidsMap.begin();
	while(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		ReportPkg.WriteShort(pserver->svid);
		ReportPkg.WriteString(pserver->ip);
		ReportPkg.WriteShort(pserver->port);
		it++;
	}
}


int ServerManager::getAllServer(GameServer** p, int* count, int level, bool isredis)
{
	_NOTUSED(level);
	svr_count = 0;
	//不要从reids中取出来，则从内存中取出来
	if(!isredis)
	{
		map<int , GameServer*>::iterator it = _svidsMap.begin();
		while( it != _svidsMap.end())
		{
			GameServer* pserver = it->second;
			servers[svr_count++] = *pserver;
			++it;
		}
		*count = svr_count;
		*p = servers;
		return 0;
	}

	redisReply* reply = RedisConnector->CommandV("KEYS Server:*");
    if(reply)
	{	
		int ret = 0;
		if(reply->type == REDIS_REPLY_ERROR)
		{
			_LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n",__FILE__,__LINE__,reply->type,reply->str);
			ret = -1;
		}
		else if (reply->type == REDIS_REPLY_ARRAY) 
		{
			for (unsigned int j = 0; j < reply->elements; j++) {
				if(reply->element[j]->type!=REDIS_REPLY_NIL)
				{
					servers[svr_count].svid = atoi(&reply->element[j]->str[7]);
					if(getServer(servers[svr_count].svid,  &servers[svr_count])==0){
						svr_count++;
					}
					else
						printf("get server error %d\n",servers[svr_count].svid);
					
				}
			}
			ret = 0;
		}
		freeReplyObject(reply);
		*count =  svr_count;
		*p = servers; 
		return ret;
	}
	else
		return -1;
}

GameServer* ServerManager::getServer(int id)
{
	map<int , GameServer*>::iterator it = _svidsMap.find(id);
	if(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		return pserver;
	}
	else
		return NULL;
}

int ServerManager::getServer(int id,  GameServer* server, bool isredis)
{
	if(server==NULL)
		return -1;
	//memset(server,0,sizeof(GameServer));
	if(!isredis)
	{
		map<int , GameServer*>::iterator it = _svidsMap.find(id);
		if(it != _svidsMap.end())
		{
			GameServer* pserver = it->second;
			server->svid = pserver->svid;
			//strcpy(server->ip,pserver->ip);
			server->ip = pserver->ip;
			server->port = pserver->port;
			server->level = pserver->level;
			server->status = pserver->status;
			server->priority = pserver->priority;
			server->maxUserCount = pserver->maxUserCount;
			server->maxTabCount = pserver->maxTabCount;
			server->currUserCount = pserver->currUserCount;
			server->currTabCount = pserver->currTabCount;
			server->lasttime = pserver->lasttime;
			server->gameID = pserver->gameID;
			return 0;
		}
		else
		{
			_LOG_WARN_("[ServerManager:getServer] Can't Find Server in Process Memory For id[%d]\n", id);
			return -1;
		}
	}

	redisReply* reply = RedisConnector->CommandV("HMGET Server:%d   "
														"svid ip port level status priority maxUserCount maxTabCount currUserCount currTabCount lasttime", id);
    if(reply)
	{	
		server->svid = id;
		if (reply->type == REDIS_REPLY_ARRAY) 
		{
			if(reply->elements==0){
				_LOG_ERROR_("[%s:%d] Can't get server[%d],server maybe not add\n",__FILE__,__LINE__, id);
				freeReplyObject(reply);
				return   -1; 
			}
			else
			{
				for (unsigned int j = 0; j < reply->elements; j++) 
				{
					if(reply->element[j]->type==REDIS_REPLY_NIL){
						freeReplyObject(reply);
						return   -1; 
					}
					switch (j)
					{
						case 0:server->svid = atoi(reply->element[j]->str);break;
						case 1:
						{
							if (reply->element[j]->str != NULL)
							{
								server->ip = reply->element[j]->str;
							}
							break;
						}
						case 2: server->port = atoi(reply->element[j]->str);break;
						case 3: server->level = atoi(reply->element[j]->str);break;
						case 4: server->status = atoi(reply->element[j]->str);break;
						case 5: server->priority = atoi(reply->element[j]->str);break;
						case 6: server->maxUserCount = atoi(reply->element[j]->str);break;
						case 7: server->maxTabCount = atoi(reply->element[j]->str);break;
						case 8: server->currUserCount = atoi(reply->element[j]->str);break;
						case 9: server->currTabCount = atoi(reply->element[j]->str);break;
						case 10: server->lasttime = atoi(reply->element[j]->str);break;
						case 11: server->gameID = atoi(reply->element[j]->str);break;  //add by maoshuoqiong 20160808
					}
				}
			}
			freeReplyObject(reply);
			return 0;
		
		}else if(reply->type == REDIS_REPLY_ERROR){
			_LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n",__FILE__,__LINE__,reply->type,reply->str);
			return -1;
		}
	}
	else
		return -1;

	return -1;
}

int ServerManager::setServerStatus(int id, int status)
{
	map<int , GameServer*>::iterator it = _svidsMap.find(id);
	if(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		pserver->status = status;
	}
	else
		_LOG_WARN_("[ServerManager:setServerStatus] Can't Find Server in Process Memory For id[%d]\n", id);
	return setServerProperty(id, "status", status);
}

int ServerManager::setServerPriority(int id, int priority)
{
	map<int , GameServer*>::iterator it = _svidsMap.find(id);
	if(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		pserver->priority = priority;
	}
	else
		_LOG_WARN_("[ServerManager:delServer] Can't Find Server in Process Memory For id[%d]\n", id);
	return setServerProperty(id, "priority", priority);
}

int ServerManager::getGameTypes(int* games)
{
	int count = 0;
	redisReply* reply = RedisConnector->CommandV("HGET Statistics All");
	//_LOG_WARN_("type:%d\n",reply->type);
	if(reply)
    {
		if(reply->type == REDIS_REPLY_ERROR)
        {
            _LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n",__FILE__,__LINE__,reply->type,reply->str);
        }
		else if(reply->type == REDIS_REPLY_NIL)
		{
			_LOG_WARN_("[%s:%d] [REDIS_REPLY_NIL]:[HGET Statistics AllTypes]\n",__FILE__,__LINE__);
		}
		else if(reply->type == REDIS_REPLY_STRING)
        {
			vector<std::string> vlist = split(std::string(reply->str), ",", true, "");
			count = vlist.size();
			for(int i=0; i<count; i++)
			{
				games[i] = atoi(vlist[i].c_str());	
			}
        }
        else if (reply->type == REDIS_REPLY_ARRAY)
        {
            for (unsigned int j = 0; j < reply->elements; j++) {

                if(reply->element[j]->type!=REDIS_REPLY_NIL)
                {
                    switch (j)
                    {
                    }
                }
            }
        }
		freeReplyObject(reply);
	}
	//freeReplyObject(reply);
	return count;
}

int ServerManager::getCurrUserNum(int level, int* basenum)
{
	int currNum = 0;
	int baseNum = 0;
	redisReply* reply = RedisConnector->CommandV("HMGET Statistics Server:%02d.num Server:%02d.base ",level,level );
    if(reply)
	{	
		if(reply->type == REDIS_REPLY_ERROR)
		{
			_LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n",__FILE__,__LINE__,reply->type,reply->str);
		}
		else if (reply->type == REDIS_REPLY_ARRAY) 
		{
			for (unsigned int j = 0; j < reply->elements; j++) {
				
				if(reply->element[j]->type!=REDIS_REPLY_NIL)
				{
					switch (j)
					{
						case 0: currNum = atoi(reply->element[j]->str);break;
						case 1: baseNum = atoi(reply->element[j]->str);break;
					} 
				}
			}
		}
		freeReplyObject(reply);
	}

	*basenum = baseNum;
	return currNum;
}


int ServerManager::getCurrUserNum(E_SERVER_TYPE gameid,E_GAME_LEVEL_ID level)
{
	int currNum = 0;
	redisReply* reply = RedisConnector->CommandV("HMGET Game:%d %d",gameid,level );
	if(reply)
	{	
		if(reply->type == REDIS_REPLY_ERROR)
		{
			_LOG_ERROR_("[%s:%d] Redis_Error[%d][%s]\n",__FILE__,__LINE__,reply->type,reply->str);
		}
		else if (reply->type == REDIS_REPLY_INTEGER) 
		{
			currNum = reply->integer;
		}
		//else if (reply->type == REDIS_REPLY_ARRAY) 
		//{
		//	for (unsigned int j = 0; j < reply->elements; j++) {

		//		if(reply->element[j]->type!=REDIS_REPLY_NIL)
		//		{
		//			switch (j)
		//			{
		//			case 0: currNum = atoi(reply->element[j]->str);break;
		//			case 1: baseNum = atoi(reply->element[j]->str);break;
		//			} 
		//		}
		//	}
		//}
		freeReplyObject(reply);
	}
	return currNum;
}


int ServerManager::updateCurrUserNum(GameServer* server)
{
	redisReply* reply = RedisConnector->CommandV("HSET Game:%d %d %d",server->gameID,server->level,server->currUserCount);
	if(reply)
	{
		freeReplyObject(reply);
		return 0;
	}
	else
		return -1;
}

int ServerManager::setServerProperty(int id, const char* field,const char* value)
{
	redisReply* reply = RedisConnector->CommandV("HSET Server:%d %s %s ", id , field, value);
    if(reply)
	{
		freeReplyObject(reply);
		return 0;
	}
	else
		return -1;
}

int ServerManager::setServerProperty(int id, const  char* field, int  value)
{
	redisReply* reply = RedisConnector->CommandV("HSET Server:%d %s %d", id , field, value);
    if(reply)
	{
		freeReplyObject(reply);
		return 0;
	}
	else
		return -1;
}

int ServerManager::addGameServerHandler(int id, CDLSocketHandler* p)
{
	ClientHandler* handler = (ClientHandler*) p ;
	handler->setID(id);
	handler->setHandleType(GMSRV_HANDLE);
	serversMap[id] = handler;
	return 0;
}
void ServerManager::delGameServerHandler(int id)
{
	serversMap.erase(id);
}

CDLSocketHandler* ServerManager::getGameServerHandler(int id)
{
	 map<int , CDLSocketHandler*>::iterator it = serversMap.find(id);
	 if(it == serversMap.end())
		 return NULL;
	 else
		 return it->second;

}


int GET(GameServer* s, int k)
{ 
	switch(k)
	{
		case SERVER_SVID: return s->svid;
		case SERVER_PRIORITY: return s->priority;
		case SERVER_USER: return s->currUserCount;
		case SERVER_TYPE: return s->level;
		default:  return s->svid;
	}
}

//冒泡排序  0 DESC 1 ASC
void BubbleSort(GameServer* serverlist[], int size , int fieldName, int flag )  
{  
    GameServer* temp;  
	if(flag==0)
	{
		for (int i=size-1;i>0;i--)  
		{  
			for (int j=0;j<i;j++)  
			{  
				if ( GET(serverlist[j],fieldName) < GET(serverlist[j+1],fieldName) )  
				{  
					temp = serverlist[j];  
					serverlist[j] = serverlist[j+1];  
					serverlist[j+1] = temp;  
				}  
			}  
		}  
	}
	else
	{
		for (int i=size-1;i>0;i--)  
		{  
			for (int j=0;j<i;j++)  
			{  
				if ( GET(serverlist[j],fieldName) > GET(serverlist[j+1],fieldName) )  
				{  
					temp = serverlist[j];  
					serverlist[j] = serverlist[j+1];  
					serverlist[j+1] = temp;  
				}  
			}  
		}  
	}
}

//快速排序 0 DESC 1 ASC
void QuickSort(GameServer* serverlist[]  , int left, int right, int fieldName, int flag=0)
{
        int i(left),j(right);
        int middle = 0 ;
		//middle = atof((*plist)[(left+right)/2]->UpDownRate);//求中间值
        middle = GET(  serverlist[(left+right)/2]  , fieldName);
		 
        do{

			if(flag==0)
			{
                while((GET( serverlist[i],fieldName)>middle)&&(i<right))//从左扫描大于中值的数
                                        i++;
                while((GET(serverlist[j] ,fieldName)<middle) && (j>left))//从右扫描小于中值的数
			                           j--;
			}
			else
			{
				 while((GET( serverlist[i],fieldName)<middle)&&(i<right)) 
                                        i++;
                while((GET(serverlist[j] ,fieldName)>middle) && (j>left)) 
			                           j--;
			}
				//找到了一对值,交换
				if(i<=j)
				{
						GameServer*  temp =   serverlist[i];
						 serverlist[i] =  serverlist[j];
						 serverlist[j] = temp;
						i++;
						j--;
				}
        }while(i<=j);//如果两边扫描的下标交错，就停止（完成一次）

        //当左边部分有值(left<j)，递归左半边
        if(left<j)
			QuickSort(serverlist,left,j,fieldName,flag);
		//当右边部分有值(right>i)，递归右半边
        if(right>i)
			QuickSort(serverlist,i,right,fieldName,flag);
}


void orderServerBySvid(GameServer* serverlist[], int size)
{
	BubbleSort(serverlist, size ,SERVER_SVID,1);
}

void orderServerByType(GameServer* serverlist[], int size )
{
	BubbleSort(serverlist, size ,SERVER_TYPE,1);
}

void orderServerByPriority(GameServer* serverlist[], int size )
{
	BubbleSort(serverlist, size ,SERVER_PRIORITY);
}


void orderServerByUser(GameServer* serverlist[], int size )
{
	BubbleSort(serverlist, size ,SERVER_USER);
}


void filterServerByType(GameServer* servers, int count, int level, GameServer* serverlist[], int* size)
{
		*size = 0;
		for(int i=0 ; i< count;i++)
		{
			GameServer* server =  &servers[i];
			if(server->level==level && server->status==1)
			{
				if( server->currUserCount < server->maxUserCount*0.9 )
					serverlist[(*size)++] = server;
			}
				
		}	
}

GameServer* ServerManager::getBestServer(int level,bool isredis) 
{
	GameServer* servers;
	int count = 0;
	int ret = getAllServer( &servers, &count, level, isredis);
	
	if(ret==-1 || count==0)
		return NULL;
	
	GameServer* serverlist[128];
	int size = 0;
	 
	if(level==0)
	{
		short stype = 2;
		filterServerByType(servers, count, stype, serverlist,&size); //取十分钟场
		orderServerBySvid(serverlist, size);
		orderServerByPriority(serverlist,size);
		if(size>0)
			return serverlist[0];
	}
	else
	{
		filterServerByType(servers, count, level, serverlist, &size);
	  	orderServerBySvid(serverlist, size);
		orderServerByPriority(serverlist,size);
		if(size>0)
			return serverlist[0];
	}
	return NULL;
}


void ServerManager::addCurrTabCount(int id)
{
	map<int , GameServer*>::iterator it = _svidsMap.find(id);
	if(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		pserver->currTabCount ++;
	}
}

int ServerManager::getCurrTabCount(int id)
{
	map<int , GameServer*>::iterator it = _svidsMap.find(id);
	if(it != _svidsMap.end())
	{
		GameServer* pserver = it->second;
		return pserver->currTabCount;
	}
	return 0;
}

int ServerManager::send(CDLSocketHandler* clientHandler, OutputPacket* outPacket, bool isEncrypt)
{
    if(clientHandler && outPacket)
    {
        if(isEncrypt)
            outPacket->EncryptBuffer();
        return clientHandler->Send(outPacket->packet_buf(),outPacket->packet_size());
    }
    return -1;
}

int ServerManager::sendToGmServer(int svid, OutputPacket* outPack, bool isEncrypt )
{
	CDLSocketHandler* gameHandler = getGameServerHandler(svid);
	return send( gameHandler, outPack, isEncrypt);
}

vector<string> split(const string& src,const string& tok, bool trim , string null_subst )
{
    vector<string> v;
    if( src.empty() || tok.empty() )
        return v;
    string::size_type pre_index = 0, index = 0, len = 0;
    while( (index = src.find_first_of(tok, pre_index)) != string::npos )
    {
         if( (len = index-pre_index)!=0 )
            v.push_back(src.substr(pre_index, len));
        else if(trim==false)
             v.push_back(null_subst);
         pre_index = index+1;
    }
    string endstr = src.substr(pre_index);
     if( trim==false ) v.push_back( endstr.empty()? null_subst:endstr );
     else if( !endstr.empty() ) v.push_back(endstr);
    return v;
}




