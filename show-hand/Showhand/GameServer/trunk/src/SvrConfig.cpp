#include "SvrConfig.h"
#include <string>
#include <vector>
#include "tinyxml.h"
#include "Logger.h"
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

}

SvrConfig* SvrConfig::getInstance()
{
	if(instance==NULL)
	{
		instance = new SvrConfig();
		instance->init();
	}
	return instance;
}

void SvrConfig::init()
{
	size = 0;
	parseXML("../etc/server.xml");
}

int SvrConfig::parseXML(const char* path)
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
				TiXmlElement * backElement= rootNode->FirstChildElement("moneyserver") ;
				 
				 if(backElement==NULL)
						throw "cann't find  back node" ;
				for( TiXmlNode* child1 = backElement->FirstChild(); child1!=NULL; child1=child1->NextSibling())
                 {
						//nodes
						TiXmlElement * nodesElement = child1->ToElement();
						int id  = GetAttribute(nodesElement, "id",-1);
						Nodes* backnodes= new Nodes(id);
						 //printf("node:id[%d]  \n",id );
					 
						for(   TiXmlNode* child2 = nodesElement->FirstChild(); child2!=NULL;child2=child2->NextSibling())
						{
								//server
								TiXmlElement * serverElement = child2->ToElement();

								int id = GetAttribute(serverElement, "id",-1);
								string host = GetAttribute(serverElement, "host","NULL");
								int port  = GetAttribute(serverElement, "port",-1);
								std::string is_heart_str  = GetAttribute(serverElement, "heart","false");
								bool isheart = (is_heart_str=="true");
								backnodes->add(new Node( id, host.c_str(), port, isheart));				 
								//printf("server:id[%d] host[%s] port[%d] isheart[%s]\n",id, host.c_str(), port, is_heart_str.c_str());
						}

						back_nodes[size++] = backnodes;
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


