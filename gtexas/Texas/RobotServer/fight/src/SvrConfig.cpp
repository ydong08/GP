#include "SvrConfig.h"
#include <string>
#include <vector>
#include "tinyxml.h"
#include "Logger.h"
#include "Configure.h"
using namespace std;

static  SvrConfig* instance = NULL;

std::string  GetAttribute(TiXmlElement * element,std::string name,std::string defaultValue)
{
    const char * attribute =  element->Attribute( name.c_str() ) ;

    if(attribute==NULL || std::string(attribute)=="") 
       return defaultValue;
     else
       return  std::string(attribute);
}

int  GetAttribute(TiXmlElement * element,std::string name, int defaultValue)
{
    const char * attribute =  element->Attribute( name.c_str() ) ;

    if(attribute==NULL || std::string(attribute)=="") 
       return defaultValue;
     else
       return atoi(attribute);
}

SvrConfig::SvrConfig()
{
	namesize = 0;
	linksize = 0;
	serversize = 0;
}

SvrConfig* SvrConfig::getInstance()
{
	if(instance==NULL)
	{
		instance = new SvrConfig();
	}
	return instance;
}

void SvrConfig::initServerConf()
{
	serversize = 0;
	parseServerXML(Configure::instance()->serverxml.c_str());
}

void SvrConfig::initLinkConf()
{
	for(int i = 0; i < linksize; i++)
	{
		if(linklists[i] != NULL)
		{
			delete linklists[i];
			linklists[i] = NULL;
		}
	}
	linksize = 0;
	parseLinkXML("../etc/headlink.xml");
}

void SvrConfig::initNameConf()
{
	for(int i = 0; i < namesize; i++)
	{
		if(namelists[i] != NULL)
		{
			delete namelists[i];
			namelists[i] = NULL;
		}
	}
	namesize = 0;
	parseNameXML("../etc/name.xml");
}

int SvrConfig::parseServerXML(const char* content)
{
	TiXmlDocument *myDocument = NULL;
	try
	{
		myDocument = new TiXmlDocument();
		myDocument->Parse(content);
		if(myDocument->Error())
		{
			 int i =  myDocument->ErrorId();
			 std::string error;
			 if(i==2)
					error = "file not exist: "+std::string(content);
			 else
					error = "file format error: "+std::string(content);
			 throw error ;
		}
		//back
		TiXmlElement * rootNode = myDocument->RootElement();
		TiXmlElement * backElement= rootNode->FirstChildElement("gameserver") ;
		 
		if(backElement==NULL)
			throw "cann't find  back node" ;
		for( TiXmlNode* child = backElement->FirstChild(); child!=NULL; child=child->NextSibling())
		{
			//nodes
			TiXmlElement * serverElement = child->ToElement();
			int svid  = GetAttribute(serverElement, "id",-1);
			string host = GetAttribute(serverElement, "host","NULL");
			int port  = GetAttribute(serverElement, "port",-1);				 
			//printf("server:svid[%d] host[%s] port[%d]\n",svid, host.c_str(), port);
			ServerNode* server = new ServerNode(svid,host.c_str(), port);
			serverlists[serversize++] = server;
		}
	}
	catch (std::string& e)
	{
		delete myDocument ;
		printf("err:[%s] \n",e.c_str() );
		return -1;
	}

	return 0;

}

int SvrConfig::parseLinkXML(const char* path)
{
	TiXmlDocument *myDocument = NULL;
	try
	{
		myDocument = new TiXmlDocument(path);
		if(!myDocument->LoadFile())
		{
			 int i =  myDocument->ErrorId();
			 std::string error;
			 if(i==2)
					error = "file not exist: "+std::string(path);
			 else
					error = "file format error: "+std::string(path);
			 throw error ;
		}
		//back
		TiXmlElement * rootNode = myDocument->RootElement();
		 
		if(rootNode==NULL)
			throw "cann't find  links node" ;
		for( TiXmlNode* child = rootNode->FirstChild(); child!=NULL; child=child->NextSibling())
		{
			//nodes
			TiXmlElement * serverElement = child->ToElement();
			string link = GetAttribute(serverElement, "link","http://tp3.sinaimg.cn/2305056670/50/5625981672/0");
			//printf("linksize:%d link:%s\n", linksize, link.c_str());
			LinkNode* head_link = new LinkNode(link.c_str());
			linklists[linksize++] = head_link;
		}
	}
	catch (std::string& e)
	{
		delete myDocument ;
		printf("err:[%s] \n",e.c_str() );
		return -1;
	}

	return 0;

}

LinkNode* SvrConfig::getRandLinkNode()
{
	return linklists[rand()%linksize];
}

int SvrConfig::parseNameXML(const char* path)
{
	TiXmlDocument *myDocument = NULL;
	try
	{
		myDocument = new TiXmlDocument(path);
		if(!myDocument->LoadFile())
		{
			 int i =  myDocument->ErrorId();
			 std::string error;
			 if(i==2)
					error = "file not exist: "+std::string(path);
			 else
					error = "file format error: "+std::string(path);
			 throw error ;
		}
		//back
		TiXmlElement * rootNode = myDocument->RootElement();
		 
		if(rootNode==NULL)
			throw "cann't find  back node" ;
		for( TiXmlNode* child = rootNode->FirstChild(); child!=NULL; child=child->NextSibling())
		{
			//nodes
			TiXmlElement * serverElement = child->ToElement();
			string name = GetAttribute(serverElement, "name","gmail");
			//printf("namesize:%d name:%s\n", namesize, name.c_str());
			NameNode* names = new NameNode(name.c_str());
			namelists[namesize++] = names;
		}
	}
	catch (std::string& e)
	{
		delete myDocument ;
		printf("err:[%s] \n",e.c_str() );
		return -1;
	}

	return 0;

}

NameNode* SvrConfig::getRandNameNode()
{
	return namelists[rand()%namesize];
}


