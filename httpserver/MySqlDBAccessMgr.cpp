#include "MySqlDBAccessMgr.h"

#include "RedisAccess.h"
#include "../CConfig.h"
#include "JsonValueConvert.h"

MySqlDBAccess *MySqlDBAccessMgr::DBMaster() {
	//	return m_db_access_map[db_ucenter];
	return m_db_master_conn;
}

MySqlDBAccess *MySqlDBAccessMgr::DBSlave() {
	//	return m_db_access_map[db_ucenter];
	return m_db_slave_conn;
}

MySqlDBAccess* MySqlDBAccessMgr::DBRouting()
{
	return m_db_routing_conn;
}

MySqlDBAccessMgr::MySqlDBAccessMgr()
{
	m_db_master_conn = NULL;
	m_db_slave_conn = NULL;
}

MySqlDBAccessMgr::~MySqlDBAccessMgr()
{
	if (m_db_master_conn)
	{
		delete m_db_master_conn;
		m_db_master_conn = NULL;
	}
	if (m_db_slave_conn)
	{
		delete m_db_slave_conn;
		m_db_slave_conn = NULL;
	}
	if (m_db_routing_conn)
	{
		delete m_db_routing_conn;
		m_db_routing_conn = NULL;
	}
}

void MySqlDBAccessMgr::Init()
{
	if (m_db_master_conn) return;

	CConfig& conf = CConfig::getInstance();

	//db.table 方式访问， 主从初始化
	m_db_master_conn = new MySqlDBAccess();

	m_db_master_conn->Connect(Json::SafeConverToStr(conf.m_jvDbMaster["ip"], "0.0.0.0").c_str(), 
		Json::SafeConverToInt32(conf.m_jvDbMaster["port"], MYSQL_PORT) , 
		Json::SafeConverToStr(conf.m_jvDbMaster["user"], "root").c_str(), 
		Json::SafeConverToStr(conf.m_jvDbMaster["passwd"], "root").c_str());
	m_db_slave_conn = new MySqlDBAccess();
	m_db_slave_conn->Connect(Json::SafeConverToStr(conf.m_jvDbSlave["ip"], "0.0.0.0").c_str(), 
		Json::SafeConverToInt32(conf.m_jvDbSlave["port"], MYSQL_PORT) , 
		Json::SafeConverToStr(conf.m_jvDbSlave["user"], "root").c_str(), 
		Json::SafeConverToStr(conf.m_jvDbSlave["passwd"], "root").c_str());
	m_db_routing_conn = new MySqlDBAccess();
	m_db_routing_conn->Connect(Json::SafeConverToStr(conf.m_jvDbRouting["ip"], "0.0.0.0").c_str(), 
		Json::SafeConverToInt32(conf.m_jvDbRouting["port"], MYSQL_PORT) , 
		Json::SafeConverToStr(conf.m_jvDbRouting["user"], "root").c_str(), 
		Json::SafeConverToStr(conf.m_jvDbRouting["passwd"], "root").c_str());
}