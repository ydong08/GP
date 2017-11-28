
#pragma once

#include <string>
#include <string.h>
#include <stdio.h>
#include "Util.h"

//具体的服务节点
class Node
{
	public:
		Node(int id, const char* ip, unsigned short port, bool isheart)
		{
			this->id = id;
			strcpy(this->ip,ip);
			this->port = port;
			this->isheart = isheart;
		}
		char ip[64];
		char des[128];
		unsigned short port;
		int id;
		bool isheart;
};

//每一种类型的服务
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

//Alloc服务器配置
class SvrConfig_
{
		Singleton(SvrConfig_);
	
	public:	
			void init(const std::string& path);
			int getBackNodes(){return size;};
			Nodes* getNodes(int i)
			{
				return back_nodes[i];
			}

	private:
			int parseXML(const char* path);
			Nodes* back_nodes[128];
			int size;			
};