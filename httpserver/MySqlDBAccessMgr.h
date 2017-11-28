#pragma once

#include "MySqlDBAccess.h"

class MySqlDBAccessMgr
{
public:
	MySqlDBAccessMgr();
	~MySqlDBAccessMgr();

	void Init();
	MySqlDBAccess* DBMaster();
	MySqlDBAccess* DBSlave();
	MySqlDBAccess* DBRouting();

private:
	//std::map <std::string, MySqlDBAccess*> m_db_access_map;
	MySqlDBAccess *m_db_master_conn;
	MySqlDBAccess *m_db_slave_conn;
	MySqlDBAccess *m_db_routing_conn;

	std::map<int , int> specialSitemid;
};

