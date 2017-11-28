#include "SvrConfig_.h"
#include <vector>
#include "tinyxml.h"
#include "Logger.h"
using namespace std;

static std::string  GetAttribute(TiXmlElement * element,std::string name,std::string defaultValue)
{
    const char * attribute =  element->Attribute( name.c_str() ) ;

    if(attribute==NULL || std::string(attribute)=="") 
       return defaultValue;
     else
       return  std::string(attribute);
}

static int  GetAttribute(TiXmlElement * element,std::string name, int defaultValue)
{
    const char * attribute =  element->Attribute( name.c_str() ) ;

    if(attribute==NULL || std::string(attribute)=="") 
       return defaultValue;
     else
       return atoi(attribute);
}

SvrConfig_::SvrConfig_()
{

}

SvrConfig_::~SvrConfig_()
{

}

void SvrConfig_::init(const std::string& path)
{
	size = 0;
	parseXML(path.c_str());
}

int SvrConfig_::parseXML(const char* path)
{
	TiXmlDocument *myDocument = NULL;
	try
	{
		myDocument = new TiXmlDocument(path);
		if (!myDocument->LoadFile())
		{
			int i =  myDocument->ErrorId();
			std::string error;
			if (i==2)
			{
				error = "file not exist: " + std::string(path);
			}
			else
			{
				error = "file format error: " + std::string(path);
			}
			LOGGER(E_LOG_ERROR) << error;
			throw error ;
		}
		
		//back
		TiXmlElement *rootNode = myDocument->RootElement();
		TiXmlElement *backElement= rootNode->FirstChildElement("back") ;
				 
		if(backElement==NULL)
		{
			LOGGER(E_LOG_ERROR) << "cann't find  back node";
			throw "cann't find  back node";
		}

		for( TiXmlNode* child1 = backElement->FirstChild(); child1!=NULL; child1=child1->NextSibling())
		{
			//nodes
			TiXmlElement * nodesElement = child1->ToElement();
			int id  = GetAttribute(nodesElement, "id",-1);
			Nodes* backnodes= new Nodes(id);
		

			for (TiXmlNode* child2 = nodesElement->FirstChild(); child2!=NULL; child2=child2->NextSibling())
			{
				//server
				TiXmlElement * serverElement = child2->ToElement();

				int id = GetAttribute(serverElement, "id",-1);
				string host = GetAttribute(serverElement, "host","NULL");
				int port  = GetAttribute(serverElement, "port",-1);
				std::string is_heart_str  = GetAttribute(serverElement, "heart","false");
				bool isheart = (is_heart_str=="true");
				backnodes->add(new Node( id, host.c_str(), port, isheart));				 
				LOGGER(E_LOG_INFO) << "Server: ID=" << id << " host=" << host << " port=" << port << " heart" << is_heart_str;
			}
			back_nodes[size++] = backnodes;
		}
	}
	catch (std::string& e)
	{
			delete myDocument ;
			LOGGER(E_LOG_ERROR) << "err :" << e;
			return -1;
	}
	return 0;
}