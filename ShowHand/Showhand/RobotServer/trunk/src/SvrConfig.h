#ifndef __SvrConfig_H__
#define __SvrConfig_H__
#include <string.h>

class ServerNode
{
	public:
		ServerNode(int svid, const char* ip, short port)
		{
			this->svid = svid;
			strcpy(this->ip,ip);
			this->port = port;
		}
		char ip[64];
		char des[128];
		short port;
		int svid;
};

class NameNode
{
	public:
		NameNode(const char* _name)
		{
			strcpy(this->name,_name);
		}
		char name[64];
};

class LinkNode
{
	public:
		LinkNode(const char* link)
		{
			strcpy(this->head_link,link);
		}
		char head_link[128];
};

class SvrConfig
{
	public:
		static SvrConfig* getInstance();		
		void initServerConf();
		int getServerListSize(){return serversize;};
		ServerNode* getServerNode(int i)
		{
			return serverlists[i];
		}

	private:
		SvrConfig();
		int parseServerXML(const char* path);
		
		
		ServerNode* serverlists[128];
		int serversize;

	public:
		void initLinkConf();
		int getLinkListSize(){return linksize;};
		LinkNode* getLinkNode(int i)
		{
			return linklists[i];
		}

		LinkNode* getRandLinkNode();
	private:
		int parseLinkXML(const char* path);
		LinkNode* linklists[1024];	
		int linksize;

	public:
		void initNameConf();
		int getNameListSize(){return namesize;};
		NameNode* getNameNode(int i)
		{
			return namelists[i];
		}

		NameNode* getRandNameNode();

	private:
		int parseNameXML(const char* path);
		NameNode* namelists[4096];
		int namesize;
};

#endif
