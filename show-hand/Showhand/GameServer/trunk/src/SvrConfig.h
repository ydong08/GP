#ifndef __SvrConfig_H__
#define __SvrConfig_H__
#include <string.h>

class Node
{
	public:
		Node(int id, const char* ip, short port, bool isheart)
		{
			this->id = id;
			strcpy(this->ip,ip);
			this->port = port;
			this->isheart = isheart;
		}
		char ip[64];
		char des[128];
		short port;
		int id;
		bool isheart;
};

class Nodes
{
	public:
		int id;
		Nodes(int id)
		{
			this->id = id;
			size = 0;
		}

		void add(Node* s)
		{
			serverlist[size++] = s;
		}

		Node* get(int i)
		{
			return serverlist[i];
		}

		int count(){return size;};
	private:
		Node* serverlist[128];//每个节点最多128个服务器
		int size;
};

class SvrConfig
{
	public:
			static SvrConfig* getInstance();		
			void init();
			int getBackNodes(){return size;};
			Nodes* getNodes(int i)
			{
				return back_nodes[i];
			}

	private:
			SvrConfig();
			int parseXML(const char* path);
			
			
			Nodes* back_nodes[128];
			int size;			
};

#endif
