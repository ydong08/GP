
#pragma once

#include <string>
#include <string.h>
#include <stdio.h>
#include "Util.h"

//����ķ���ڵ�
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

//ÿһ�����͵ķ���
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
		Node* serverlist[128];//ÿ���ڵ����128��������
		int size;
};

//Alloc����������
class SvrConfig_
{
		Singleton(SvrConfig_);
	
	public:	
			void init(const std::string& path);
			void initWithAddr(const char* addr);
			void initWithXmlContent(const char* content);
			int getBackNodes(){return size;};
			Nodes* getNodes(int i)
			{
				return back_nodes[i];
			}

	private:
			int parseXML(const char* xml, bool bContent = false);
			Nodes* back_nodes[128];
			int size;			
};