#include "RedisAccessMgr.h"
#include "CConfig.h"
#include "threadres.h"
#include "EvHttpSvr.h"
#include "Util.h"
#include "Helper.h"
#include "StrFunc.h"

RedisAccess *RedisAccessMgr::minfo(int mid) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_UCNF)];
}

RedisAccess *RedisAccessMgr::common(const char *mode) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_PHP)];
}

RedisAccess *RedisAccessMgr::game() {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_PARAM)];
}

RedisAccess *RedisAccessMgr::server() {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_PARAM)];
}

RedisAccess *RedisAccessMgr::account(const char *mode) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_PHP)];
}

RedisAccess *RedisAccessMgr::ote(int uid, const char *mode) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_OTE)];
}

RedisAccess *RedisAccessMgr::rank(int gameid) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_RANK)];
}

RedisAccess *RedisAccessMgr::userServer(int32_t uid, const char *mode) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_USER)];
}

RedisAccess *RedisAccessMgr::ucenter(const char *mode) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_RANK)];
}

RedisAccess *RedisAccessMgr::money(const char *mode) {
	return m_redis_access_map[CConfig::getInstance().GetRedisIpAndPort(PORT_MON)];
}

RedisAccessMgr::~RedisAccessMgr()
{
	std::map <std::string, RedisAccess*>::iterator itTmp = m_redis_access_map.begin();
	for (;itTmp != m_redis_access_map.end();itTmp++)
	{
		if(itTmp->second) delete itTmp->second;
		//		m_redis_access_map.erase(itTmp);
	}
	m_redis_access_map.clear();
}

void RedisAccessMgr::redis_ping(void* args)
{
    //LOGGER(E_LOG_DEBUG) << "ping now";
	
	//LOGGER(E_LOG_DEBUG) << "GO GO GO";
	RedisAccessMgr* this_ = (RedisAccessMgr*)args;
	for(auto it = this_->m_redis_access_map.begin(); it != this_->m_redis_access_map.end(); it++)
	{
		RedisAccess* redis = it->second;
		if (redis)
		{
			if (!redis->ping())
			{
				LOGGER(E_LOG_WARNING) << "redis disconnnect, reconnect now";
				redis->connect();
			}
		}
	}
}

void RedisAccessMgr::redis_GetWinGoldRank(void* args)
{
	LOGGER(E_LOG_WARNING) << "Hello World";

	LOGGER(E_LOG_WARNING) << Util::formatGMTTime().c_str() << "ping now";
	////string value;
	//int timestamp = Helper::strtotime(Util::formatGMTTime_Date() + " 00:00:00");
	RedisAccessMgr* this_ = (RedisAccessMgr*)args;
	if(this_  == NULL)
	{
		LOGGER(E_LOG_ERROR) << "This RedisAccessMgr Error";
		return;
	}
	RedisAccess * redis = this_->rank(0);
	if(redis == NULL)
	{
		LOGGER(E_LOG_ERROR) << "open RedisAccess rank Error";
		return;
	}


	int32_t ActivityBeginTime = 0;
	int32_t ActivityEndTime = 0;
	
	
	RedisAccess* rdMoney = this_->money();
	if(rdMoney != NULL)
	{
		std::string strActivityBeginTime;
		std::string strActivityEndTime;
		if(rdMoney->GET("ActivityBeginTime",strActivityBeginTime) && rdMoney->GET("ActivityEndTime",strActivityEndTime))
		{
		   ActivityBeginTime = atoi(strActivityBeginTime.c_str());
		   ActivityEndTime = atoi(strActivityEndTime.c_str());
		}
	}
	LOGGER(E_LOG_WARNING) << "BeignTime" << ActivityBeginTime;
	LOGGER(E_LOG_WARNING) << "EndTime" << ActivityEndTime;

	time_t t = time(0);
	if (t > ActivityEndTime)
	{
		LOGGER(E_LOG_WARNING) << "ActivityEndTime Bigger CurTime return";
		return;
	}
	std::string ActivityTimeStr = Helper::getTimeStampToYearMonthTime(ActivityBeginTime);

	//循环产品号里面所有的表
	for(int i = 1 ;i < 11 ;++i)
	{
		////1253,1255,1257
		//if (!(i == 3 || i == 5 || i == 7 || i == 9))
		//{
		//	continue;
		//}
		LOGGER(E_LOG_WARNING) << StrFormatA("Search logchip.log_member%d",i);
		const std::string strTb_log_member = StrFormatA("logchip.log_member%d",i);
		//查找当月的输赢金币情况 以游戏结束时间为准
		string sql = StrFormatA("SELECT mid,SUM(money) as AllWinMoney,Count(*) as AllWinCount FROM %s WHERE etime >= %d and etime < %d GROUP BY mid;",
			strTb_log_member.c_str(),ActivityBeginTime,ActivityEndTime);

		MySqlDBAccess* dbMaster = ThreadResource::getInstance().getDBConnMgr()->DBMaster();
		if(dbMaster != NULL)
		{
			MySqlResultSet* res = dbMaster->Query(sql.c_str());
			if (res && res->getRowNum() > 0) 
			{
				for (; res->hasNext();) 
				{
					int mid = res->getIntValue("mid");
					int AllWinMoney = res->getIntValue("AllWinMoney");
					int AllWinCount = res->getIntValue("AllWinCount");

					redisReply * pRedisReply = redis->CommandV("ZADD WinGoldRank:%s %d %d",ActivityTimeStr.c_str(),AllWinMoney,mid);
					if(pRedisReply != NULL && pRedisReply->len != 0)
					{
						freeReplyObject(pRedisReply);
					}

					const std::string strTb_uc_userinfo = StrFormatA("ucenter.uc_userinfo%d",mid%10);
					string sqlName = StrFormatA("SELECT pid,mnick,ctime FROM %s WHERE mid = %d;",
						strTb_uc_userinfo.c_str(),mid);


					MySqlResultSet* resName = dbMaster->Query(sqlName.c_str());
					if (resName && resName->getRowNum() > 0) 
					{
						if (resName->hasNext()) 
						{
							std::string mnick = resName->getCharValue("mnick");
							int pid = resName->getIntValue("pid");
							int ctime = resName->getIntValue("ctime");

							delete resName;
							//存入基础信息
							redis->CommandV("HSET User:%s:%d pid %d",ActivityTimeStr.c_str(),mid,pid);
							redis->CommandV("HSET User:%s:%d mNick %s",ActivityTimeStr.c_str(),mid,mnick.c_str());
							redis->CommandV("HSET User:%s:%d cTime %d",ActivityTimeStr.c_str(),mid,ctime);
						}
					}
					//总量存入 详细信息
					redis->CommandV("HSET User:%s:%d AllWinMoney %d",ActivityTimeStr.c_str(),mid,AllWinMoney);
					redis->CommandV("HSET User:%s:%d AllWinCount %d",ActivityTimeStr.c_str(),mid,AllWinCount);
				}
			}

			delete res;

			//查land的数据
			string sqlLand = StrFormatA("SELECT mid,SUM(money) as LandMoney,Count(*) as LandCount FROM %s WHERE etime >= %d and etime < %d and gameid = 3 GROUP BY mid;",
				strTb_log_member.c_str(),ActivityBeginTime,ActivityEndTime);
			MySqlResultSet* resLand = dbMaster->Query(sqlLand.c_str());
			if (resLand && resLand->getRowNum() > 0) 
			{
				for (; resLand->hasNext();) 
				{
					int mid = resLand->getIntValue("mid");
					int LandMoney = resLand->getIntValue("LandMoney");
					int LandCount = resLand->getIntValue("LandCount");

					//land 详细信息
					redis->CommandV("HSET User:%s:%d LandMoney %d",ActivityTimeStr.c_str(),mid,LandMoney);
					redis->CommandV("HSET User:%s:%d LandCount %d",ActivityTimeStr.c_str(),mid,LandCount);
				}
			}
			delete resLand;

			//查 flower的数据
			string sqlFlower = StrFormatA("SELECT mid,SUM(money) as FlowerMoney,Count(*) as FlowerCount FROM %s WHERE etime >= %d and etime < %d and gameid = 7 GROUP BY mid;",
				strTb_log_member.c_str(),ActivityBeginTime,ActivityEndTime);

			MySqlResultSet* resFlower = dbMaster->Query(sqlFlower.c_str());
			if (resFlower && resFlower->getRowNum() > 0) 
			{
				for (; resFlower->hasNext();) 
				{
					int mid = resFlower->getIntValue("mid");
					int FlowerMoney = resFlower->getIntValue("FlowerMoney");
					int FlowerCount = resFlower->getIntValue("FlowerCount");

					//flower 详细信息
					redis->CommandV("HSET User:%s:%d FlowerMoney %d",ActivityTimeStr.c_str(),mid,FlowerMoney);
					redis->CommandV("HSET User:%s:%d FlowerCount %d",ActivityTimeStr.c_str(),mid,FlowerCount);
				}
			}
			delete resFlower;

			//查询flower中用户3天内的充值记录
			const std::string strTb_pc_payment = StrFormatA("paycenter.pc_payment");
			int32_t iCurTime = time(NULL);
		    int32_t iThrTime = iCurTime - 3*24*3600;
			string sqlThrPay = StrFormatA("SELECT mid,pamount,ptime FROM %s WHERE ptime >= %d and ptime < %d and pstatus = 3 GROUP BY mid;",
				strTb_pc_payment.c_str(),iThrTime,iCurTime);

			MySqlResultSet* FloweResPay = dbMaster->Query(sqlThrPay.c_str());
			if (FloweResPay && FloweResPay->getRowNum() > 0) 
			{
				for (; FloweResPay->hasNext();) 
				{
					int mid = FloweResPay->getIntValue("mid");
					float pamount = FloweResPay->getFloatValue("pamount");
					int ptime = FloweResPay->getIntValue("ptime");
					char buf[16] = {0};
					sprintf(buf, "%d#%f", ptime, pamount);
					string stramountime(buf);

					//flower 充值信息详细信息
					redis->CommandV("LPUSH  PAYLIST:%d  %s",mid, stramountime);//winterSky[Add]
					redis->CommandV("EXPIRE  PAYLIST:%d"  PRIi32 " %d",mid, 3*24*3600);
				}
			}
			delete FloweResPay;

			//查 fish 的数据
			string sqlFish = StrFormatA("SELECT mid,SUM(money) as FishMoney,Count(*) as FishCount FROM %s WHERE etime >= %d and etime < %d and gameid = 50 GROUP BY mid;",
				strTb_log_member.c_str(),ActivityBeginTime,ActivityEndTime);

			//fish 详细信息

			MySqlResultSet* resFish = dbMaster->Query(sqlFish.c_str());
			if (resFish && resFish->getRowNum() > 0) 
			{
				for (; resFish->hasNext();) 
				{
					int mid = resFish->getIntValue("mid");
					int FishMoney = resFish->getIntValue("FishMoney");
					int FishCount = resFish->getIntValue("FishCount");

					//land 详细信息
					redis->CommandV("HSET User:%s:%d FishMoney %d",ActivityTimeStr.c_str(),mid,FishMoney);
					redis->CommandV("HSET User:%s:%d FishCount %d",ActivityTimeStr.c_str(),mid,FishCount);
				}
			}
			delete resFish;

			//查 niu 的数据
			string sqlNiu = StrFormatA("SELECT mid,SUM(money) as NiuMoney,Count(*) as NiuCount FROM %s WHERE etime >= %d and etime < %d and gameid = 31 GROUP BY mid;",
				strTb_log_member.c_str(),ActivityBeginTime,ActivityEndTime);

			MySqlResultSet* resNiu = dbMaster->Query(sqlNiu.c_str());
			if (resNiu && resNiu->getRowNum() > 0) 
			{
				for (; resNiu->hasNext();) 
				{
					int mid = resNiu->getIntValue("mid");
					int NiuMoney = resNiu->getIntValue("NiuMoney");
					int NiuCount = resNiu->getIntValue("NiuCount");

					//niu 详细信息
					redis->CommandV("HSET User:%s:%d NiuMoney %d",ActivityTimeStr.c_str(),mid,NiuMoney);
					redis->CommandV("HSET User:%s:%d NiuCount %d",ActivityTimeStr.c_str(),mid,NiuCount);
				}
			}

			delete resNiu;


			//const std::string strTb_pc_payment = StrFormatA("paycenter.pc_payment");
			string sqlPay = StrFormatA("SELECT mid,SUM(pamount) as payMoney FROM %s WHERE ptime >= %d and ptime < %d and pstatus = 3 GROUP BY mid;",
				strTb_pc_payment.c_str(),ActivityBeginTime,ActivityEndTime);

			MySqlResultSet* resPay = dbMaster->Query(sqlPay.c_str());
			if (resPay && resPay->getRowNum() > 0) 
			{
				for (; resPay->hasNext();) 
				{
					int mid = resPay->getIntValue("mid");
					int payMoney = resPay->getIntValue("payMoney");

					// 详细信息
					redis->CommandV("HSET User:%s:%d payMoney %d",ActivityTimeStr.c_str(),mid,payMoney);

				}
			}

			delete resPay;

			const std::string strTb_pc_withdrawals = StrFormatA("paycenter.pc_withdrawals");
			string sqlWithdrawals = StrFormatA("SELECT mid,SUM(pamount) as withdrawals FROM %s WHERE ptime >= %d and ptime < %d and pstatus = 5 GROUP BY mid;",
				strTb_pc_withdrawals.c_str(),ActivityBeginTime,ActivityEndTime);

			MySqlResultSet* reswWithdrawals = dbMaster->Query(sqlWithdrawals.c_str());
			if (reswWithdrawals && reswWithdrawals->getRowNum() > 0) 
			{
				for (; reswWithdrawals->hasNext();) 
				{
					int mid = reswWithdrawals->getIntValue("mid");
					int withdrawals = reswWithdrawals->getIntValue("withdrawals");

					// 详细信息
					redis->CommandV("HSET User:%s:%d withdrawals %d",ActivityTimeStr.c_str(),mid,withdrawals);
				}
			}

			delete reswWithdrawals;

		}
	}
	

}

void RedisAccessMgr::Init()
{

	if(m_redis_access_map.size() != 0) return;

	CConfig& conf = CConfig::getInstance();
	RedisAccess *redisTmp = NULL;
	for (int i = 0 ; i < PORT_CNT; i++)
	{
		redisTmp = new RedisAccess(conf.redis_conf[i].ip, conf.redis_conf[i].port);
		redisTmp->connect();
		m_redis_access_map[conf.GetRedisIpAndPort(i)] = redisTmp;
	}
	// 添加定时器
	struct timeval tvTimer = {10,0};
	EvTimerParam evtTimer;
	evtTimer.tvTimer = tvTimer;
	evtTimer.timerCb = redis_ping;
	evtTimer.context = this;
	CEvHttpSvr::getInstance().AddThreadTimer(pthread_self(),evtTimer);

	struct timeval tvTimerWinGoldRank = {1800,0};
	EvTimerParam evtTimerWinGoldRank;
	evtTimerWinGoldRank.tvTimer = tvTimerWinGoldRank;
	evtTimerWinGoldRank.timerCb = redis_GetWinGoldRank;
	evtTimerWinGoldRank.context = this;
	CEvHttpSvr::getInstance().AddThreadTimer(pthread_self(),evtTimerWinGoldRank);
}
