#include "RedisLogic.h"

//#include "RedisAccess.h"
#include "Logger.h"
#include "StrFunc.h"
#include "Util.h"
#include "md5.h"
#include <inttypes.h>
#include "JsonValueConvert.h"
#include "Helper.h"
#include "Lib_Ip.h"
#include "CConfig.h"
#include "SqlLogic.h"
#include "TcpMsg.h"

namespace RedisLogic
{
	static std::vector<std::string> province_ch = {
		"黑龙江" ,
		"吉林" ,
		"辽宁" ,
		"江苏" ,
		"山东" ,
		"安徽" ,
		"河北" ,
		"河南" ,
		"湖北" ,
		"湖南" ,
		"江西" ,
		"陕西" ,
		"山西" ,
		"四川" ,
		"青海" ,
		"海南" ,
		"广东" ,
		"贵州" ,
		"浙江" ,
		"福建" ,
		"台湾" ,
		"甘肃" ,
		"云南"
	};

//	static int getRoomConfigKey(int gameid, std::string& key, std::string& many);

	bool test()
	{
	/*	std::string key, many;
		getRoomConfigKey( 2, key, many);
		*/
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return false;
		}
		RedisAccess* redis = redisMgr->common();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return false;
		}

		//redisReply* reply = redis->CommandV("HMSET TEST|123321|12321 %s %s %s %s %s %s","key1","v alue1","key 2","value2","key3","");
		std::vector<const char *> vtAll;
		vtAll.push_back("HMSET");
		vtAll.push_back("TEST|123321|12321");
		vtAll.push_back("key1");
		vtAll.push_back("v alue1");
		vtAll.push_back("key 2");
		vtAll.push_back("value2");
		vtAll.push_back("key3");
		vtAll.push_back("");

		vtAll.push_back("key4");
		vtAll.push_back("value4");
		std::string strKey5 = "key5";
		std::string strvalue5 = "value5";
		vtAll.push_back(strKey5.c_str());
		vtAll.push_back(strvalue5.c_str());
		vtAll.push_back("key6");
		vtAll.push_back("''");
		vtAll.push_back("key7");
		vtAll.push_back("'中国\t广东\t阳江'");
		redisReply* reply = redis->CommandArgv(vtAll);
		if (reply == NULL)
		{
			return false;
		}

		freeReplyObject(reply);
		return true;
	}

	RedisAccess* GetRdMinfo( int mid/*=0*/ )
	{
		ThreadResource::getInstance().getRedisConnMgr()->minfo(mid);
	}

	RedisAccess* GetRdCommon( const char* mode /*= "master"*/ )
	{
		ThreadResource::getInstance().getRedisConnMgr()->common(mode);
	}

	RedisAccess* GetRdGame()
	{
		ThreadResource::getInstance().getRedisConnMgr()->game();
	}

	RedisAccess* GetRdServer()
	{
		ThreadResource::getInstance().getRedisConnMgr()->server();
	}

	RedisAccess* GetRdAccount( const char* mode /*= "master"*/ )
	{
		ThreadResource::getInstance().getRedisConnMgr()->account(mode);
	}

	RedisAccess* GetRdOte( int uid, const char* mode /*= "master"*/ )
	{
		ThreadResource::getInstance().getRedisConnMgr()->ote(uid, mode);
	}

	RedisAccess* GetRdRank( int gameid )
	{
		return ThreadResource::getInstance().getRedisConnMgr()->rank(gameid);
	}

	RedisAccess* GetRdUserServer( int32_t uid, const char* mode /*= "master"*/ )
	{
		ThreadResource::getInstance().getRedisConnMgr()->userServer(uid, mode);
	}

	RedisAccess* GetRdUcenter( const char* mode /*= "master"*/ )
	{
		ThreadResource::getInstance().getRedisConnMgr()->ucenter(mode);
	}

	RedisAccess* GetRdMoney( const char* mode /*= "master"*/ )
	{
		return ThreadResource::getInstance().getRedisConnMgr()->money(mode);
	}

/*	bool SetRegAliPayInfo(int uid, CBindAliPayInfo& info)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL) return false;
		RedisAccess* redisAcc = redisMgr->minfo();
		if (redisAcc == NULL) return false;

		redisReply* reply = redisAcc->CommandV("HMSET UCNF%d mid %d gameid %d alipay %s aliname %s ", 
			uid, uid, info. gameid, info.alipay.c_str(), info.aliname.c_str());
		if (reply == NULL)
		{
			_LOG_ERROR_("Failed to set [%d, %d, %s, %s] into UCNF redis !", 
				uid, info.gameid, info.alipay.c_str(), info.aliname.c_str());
			return false;
		}

		freeReplyObject(reply);
		return true;
	}

	bool GetRegAliPayInfo(int uid, CBindAliPayInfo& info)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL) return false;
		RedisAccess* redisAcc = redisMgr->minfo();
		if (redisAcc == NULL) return false;

		redisReply* reply = redisAcc->CommandV("HMGET UCNF%d mid gameid alipay aliname ", uid) ;
		if(reply != NULL && reply->type == REDIS_REPLY_ARRAY && reply->elements >= 4)
		{
			if(reply->element[0]->type == REDIS_REPLY_NIL)
			{
				_LOG_ERROR_("Redis cache not fit \n") ;
				freeReplyObject(reply);
				return false ;
			}

			info.mid = atoi(reply->element[0]->str) ;
			if(info.mid != uid)
			{
				_LOG_ERROR_("Redis cache data error, uid in cache is %d, current uid is %d \n", info.mid, uid) ;
				freeReplyObject(reply);
				return false ;
			}

			if (reply->element[1]->type == REDIS_REPLY_STRING)
			{
				info.gameid = atoi(reply->element[1]->str);
			}
			if (reply->element[2]->type == REDIS_REPLY_STRING)
			{
				info.alipay = reply->element[2]->str;
			}
			if (reply->element[3]->type == REDIS_REPLY_STRING)
			{
				info.aliname = reply->element[3]->str ;
			}

			if(info.alipay == "" || info.aliname == "")
			{
				freeReplyObject(reply);
				return false ;
			}
		}

		freeReplyObject(reply);
		return false ;
	}*/

	int setMidWithCacheKey(std::string& cacheKey, int mid, bool v1, bool v2)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return -1;
		}
		RedisAccess* redis = redisMgr->minfo();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return -1;
		}

		std::string vbuff = StrFormatA("%d", mid);
		//if (!redis->HSET(cacheKey.c_str() , "mid" , vbuff.c_str()))
        if (!redis->SET(cacheKey.c_str(), mid))
		{
			return -1;
		}

		return 0;
	}

	int getMidByCacheKey( std::string& cacheKey)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return -1;
		}
		RedisAccess* redis = redisMgr->minfo();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be inited when server starting!";
			return -1;
		}
		std::vector<std::string> vsFields;
		//vsFields.push_back("mid");
        //std::map<std::string,std::string> mpRet;
        std::string value;
        if (!redis->GET(cacheKey.c_str(), value))
		//if(!redis->HMGet(cacheKey.c_str(),vsFields,mpRet))
		{
			return -1;
		}
		//if (mpRet.find("mid") != mpRet.end())
		{
			return atoi(value.c_str());
		}

		//return -1;
	}

	int limitCount(string mid, string type, int lcount, bool update,int expire) 
	{
		if(mid.empty()) return 0;

		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return 0;
		}
		RedisAccess* redis = redisMgr->common();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return 0;
		}

		string cacheKey = StrFormatA("ULT|%s|%s", mid.c_str(), type.c_str());
		if (type == "reg" || type == "ip_reg" || type == "uuid") 
		{
			cacheKey = StrFormatA("%s|%s", cacheKey.c_str(), Util::formatGMTTime_Date().c_str());
		}
		string value;
		bool retcode = redis->GET(cacheKey.c_str(), value);
		int lvalue = 0;
		if (retcode) lvalue =  atoi(value.c_str());

		if ( update == true ) 
		{
			lvalue += lcount;
			redis->SET(cacheKey.c_str(), lvalue, expire); //需要加则递增.确保设置该Key
		}

		return lvalue;
	}

	int getServerIdCode(const std::string &phoneno)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return -1;
		}
		RedisAccess* redis = redisMgr->common();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return -1;
		}
		std::string key = "CDC|" + phoneno;
		std::string svrId;
		if (!redis->GET(key.c_str(), svrId))
		{
			return -1;
		}
		return atoi(svrId.c_str());
	}

	//************************************
	// Method:    GetUserInfo	//获取用户信息。注意:读出来的全部是String类型的，直接用asInt()之类会报错
	// FullName:  RedisAccessMgr::GetUserInfo
	// Access:    public 
	// Returns:   bool		//true 成功获取到数据
	// Qualifier:
	// Parameter: int32_t nMid			// 用户id
	// Parameter: Json::Value & info	//返回获取到的json
	//************************************
	bool GetUserInfo(int32_t nMid, Json::Value& info)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return false;
		}
		RedisAccess* redis = redisMgr->minfo();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return false;
		}
	
		redisReply* reply = redis->CommandV("HGETALL UCNF%" PRIi32, nMid);
		if(reply == NULL)
		{
			return false;
		}

		std::string strKeyName = "";
		if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0)
		{
			for (unsigned short j = 0; j < reply->elements; j++) 
			{
				if(reply->element[j]->type==REDIS_REPLY_NIL)
				{
					freeReplyObject(reply);
					return false; 
				}
				if(!(j & 1))
				{
					strKeyName = reply->element[j]->str;
				}
				else
				{
					if(!strKeyName.empty()) info[strKeyName] = reply->element[j]->str;
				}
			}
			freeReplyObject(reply);
			return true;
		}

		freeReplyObject(reply);
		return false;
	}

	bool SetUserInfo(int32_t nMid, Json::Value& info, uint32_t uTimeout)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return false;
		}
		RedisAccess* redis = redisMgr->minfo();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return false;
		}

		//std::string strInsert = "";
		Json::Value::Members mem = info.getMemberNames();
		map<std::string,std::string> mapKV;
		std::string strValue;
		for (Json::Value::Members::iterator itMen = mem.begin(); itMen != mem.end(); itMen++)
		{
			auto itype = info[*itMen].type();
			//LOGGER(E_LOG_INFO) << "name = " << *itMen << " type = " << itype << " value = " << info[*itMen].toStyledString();
			if (itype == Json::objectValue || itype == Json::arrayValue) continue;  
			
			strValue = TrimStr(info[*itMen].asString());
			//if (strValue.empty()) strValue = "''";

            mapKV[*itMen] = strValue;
            //LOGGER(E_LOG_INFO) << "key = " << *itMen << " str = " << str;
		}
		//if (strInsert.empty()) return false;
    
		//LOGGER(E_LOG_INFO) << strInsert;
		//redisReply* reply = redis->exeCommand("HMSET UCNF%d %s", nMid, strInsert.c_str());
        std::string hash = StrFormatA("UCNF%d", nMid);
        redis->HMSET(hash.c_str(), mapKV);

		//freeReplyObject(reply);
		redisReply* tempreply = redis->CommandV("EXPIRE  UCNF%" PRIi32 " %d", nMid , 86400);
		if(tempreply == NULL) return false; 
		freeReplyObject(tempreply);

		return true;
	}

	int codeCheck(std::string phoneno) 
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return -1;
		}
		RedisAccess* redis = redisMgr->common();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return -1;
		}
		int server_idcode = -1;
		redisReply *reply = redis->CommandV("get CDC|%s", phoneno.c_str());
		if (reply && reply->type == REDIS_REPLY_STRING)
			server_idcode = atoi(reply->str);
		freeReplyObject(reply);
		return server_idcode;
	}

	bool setCodeCheck(std::string phoneno, int idcode) 
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return false;
		}
		RedisAccess* redis = redisMgr->common();
		if (!redis)
		{
			LOGGER(E_LOG_ERROR) << "minfo redis is null, it did not be initted when server starting!";
			return false;
		}
		bool retcode = false;
		redisReply *reply = redis->CommandV("set CDC|%s %d ex %d", phoneno.c_str(), idcode, 10*60);
		if (reply && reply->type == REDIS_REPLY_STATUS && !strcmp(reply->str, "OK"))
			retcode = true;
		freeReplyObject(reply);
		return retcode;
	}
	/**
     * 
     * 登陆时分配密钥给用户
     * @param int $mid
     */
	std::string setMtkey(int32_t mid , std::string& dev_no, std::string& ip)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return "";
		}
		RedisAccess* redis_common = redisMgr->common();
		RedisAccess* redis_minfo = redisMgr->minfo();
		if (!redis_common || !redis_minfo)
		{
			LOGGER(E_LOG_ERROR) << "minfo or common redis is null, it did not be initted when server starting!";
			return "";
		}

		std::string mtkey = "";
		std::string userInfoKey = StrFormatA("UCNF%d" , mid);
		std::string flag_key = StrFormatA("%s|mtkey" , dev_no.c_str());
		std::string flag;
		bool bSetRedis = false;
		time_t now = time(NULL);
		std::string tmp_mtKey = StrFormatA("%d%d$#@!^" , now , mid);
		MD5 md5(tmp_mtKey);
		if (!redis_common->GET(flag_key.c_str() , flag) || atoi(flag.c_str()) == 0)
		{
			mtkey = md5.md5();
			bSetRedis = true;
		}
		else
		{
			if (!redis_minfo->HGET(userInfoKey.c_str() , "mtkey" , mtkey))
			{
				mtkey = md5.md5();
				bSetRedis = true;
			}
		}
		if(bSetRedis)
		{
			if (!redis_minfo->HSET(userInfoKey.c_str() , "mtkey" , mtkey.c_str()))
			{
				return "";
			}

			if (!redis_common->SET(flag_key.c_str() , 1 , 86400))
			{
				return "";
			}
		}
		if (!redis_minfo->HSET(userInfoKey.c_str() , "ip" , ip.c_str()))
		{
			return "";
		}

		return mtkey;
	}

	

	int  doTodayFirst(Json::Value& user)
	{
		return SqlLogic::addLogin(user);
	}

	void unsetUserFiled(Json::Value& jvUser)
	{
		std::string strUnsetFiled[] = { "bankPWD", "device_no", "ipArr", "mtype", "deviceToken", "aMactivetime", "aMentercount", "aMtime", "aPid", "aVersions" 
			, "devicename", "osversion", "nettype", "vendor", "gameid", "cid", "pid", "ctype", "versions", "mtime", "mstatus", "country", "province", "city"
			, "equipment_model", "networking", "" };
		int i = 0;
		while (!strUnsetFiled[i].empty())
		{
			if (!jvUser[strUnsetFiled[i]].isNull()) jvUser.removeMember(strUnsetFiled[i]);
			i++;
		}
	}

	int getLoginReward(int days)
	{
		static int login_reward[7] = { 500 , 800 , 900 , 1000 , 1500 , 2000 , 2500 };
		if (days > 7)
		{
			days = 7;
		}

		if (days <= 7 || days >= 1)
		{
			return login_reward[days - 1];
		}

		return 0;
	}
	
	void getSingleByGameid(Json::Value& user)
	{
		LOGGER(E_LOG_INFO) << "getSingleByGameid";

		Json::Reader jReader(Json::Features::strictMode());
		Json::Value jMactivetime;
		std::string strMactivetime = user["mactivetime"].asString();
		std::string productid = Json::SafeConverToStr(user["productid"]);

		LOGGER(E_LOG_INFO) << "strMactivetime";
		if (jReader.parse(strMactivetime , jMactivetime))
		{
			user["aMactivetime"] = jMactivetime;
			user["mactivetime"] = Json::SafeConverToInt32(jMactivetime[productid]);
		}
		else
		{
			user.removeMember("aMactivetime");
			user.removeMember("mactivetime");
			//   _LOG_WARN_("[HttpSvr:handleReq] recv error json[%s]\n" , strMactivetime.c_str());
		}

		LOGGER(E_LOG_INFO) << "jMentercount";
		Json::Value jMentercount;
		std::string strMentercount = user["mentercount"].asString();
		if (jReader.parse(strMentercount , jMentercount))
		{
			user["aMentercount"] = jMentercount;
			user["mentercount"] = Json::SafeConverToInt32(jMentercount[productid]);
		}
		else
		{
			user.removeMember("aMentercount");
			user.removeMember("mentercount");
			// _LOG_WARN_("[HttpSvr:handleReq] recv error json[%s]\n" , strMentercount.c_str());
		}

		LOGGER(E_LOG_INFO) << "jMtime";
		Json::Value jMtime;
		std::string strMtime = user["mtime"].asString();
		if (std::string::npos != strMtime.find("\"") && !jReader.parse(strMtime , jMtime))
		{
			user["aMtime"] = jMtime;
			user["mtime"] = Json::SafeConverToInt32(jMtime[productid]);
		}
		else
		{
			user.removeMember("aMtime");
			user.removeMember("mtime");
			//_LOG_WARN_("[HttpSvr:handleReq] recv error json[%s]\n" , strMtime.c_str());
		}
	}

	Json::Value GetHallConf(int nProductId)
	{
		Json::Value conf;
		std::vector<ip_port_conf>* hall_infos = CConfig::getInstance().getHallConf(nProductId);
        if (NULL == hall_infos)
        {
            // 产品没有单独配置大厅， 采用默认大厅配置
            hall_infos = CConfig::getInstance().getHallConf(0);
            if (NULL == hall_infos)
            {
                return conf;
            }
        }

		std::vector<ip_port_conf>::iterator it = hall_infos->begin();
		for (; it != hall_infos->end(); ++it)
		{
			if (!it->ip.empty() && it->port > 0)
			{
				conf["ip"] = it->ip;
				conf["port"] = it->port;
				return conf;
			}

			//hall_addrs.append(conf);
		}
		return conf;
	}

	int doUserInfo(Json::Value& user)
	{
		int32_t mstatus = Json::SafeConverToInt32(user["mstatus"]);
		if (mstatus == 1)
		{
			return 106;  //被封号
		}
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return -1;
		}
		int32_t mid = Json::SafeConverToInt32(user["mid"]);

		RedisAccess* redis_account = redisMgr->account();
		RedisAccess* redis_ote = redisMgr->ote(mid);
		RedisAccess* redis_common = redisMgr->common();
		RedisAccess* redis_minfo = redisMgr->minfo();
		RedisAccess* redis_ucenter = redisMgr->ucenter();

		if (!redis_account || !redis_ote || !redis_common || !redis_minfo || !redis_ucenter)
		{
			LOGGER(E_LOG_ERROR) << "redis is null, it did not be initted when server starting!";
			return -1;
		}

		std::string OTE_account_key = StrFormatA("OTE|%d" , mid); 
		std::map< std::string , std::string > accountInfo;
		redis_ote->HGetAll(OTE_account_key.c_str(), accountInfo);
	/*	if (!redis_ote->HGetAll(OTE_account_key.c_str(), accountInfo))
		{
			LOGGER(E_LOG_ERROR) << "get account info from redis failed";
			return -1;
		}*/
	/*		燕子说不需要了
		std::string strBanAccKey = StrFormatA("BAT|%" PRIi32,mid);
		std::string strBanAccRet;
		if (!redis_account_slave->GET(strBanAccKey.c_str(),strBanAccRet))
		{
			std::string strDeviceNo = user["device_no"].asString();
			strBanAccKey = StrFormatA("BAT|%s",strDeviceNo.c_str());
			if (redis_account_slave->GET(strBanAccKey.c_str(),strBanAccRet))
			{
				user["mstatus"] = atoi(strBanAccRet.c_str());//机器码状态
			}
		}
		else
		{
			user["mstatus"] = atoi(strBanAccRet.c_str()); //账号状态
		}
		if (0 != Json::SafeConverToInt32(user["mstatus"]) || 1 == atoi(accountInfo["bat"].c_str()))
		{
			return 106;  //被封号
		}
	*/
		LOGGER(E_LOG_INFO) << "getSingleByGameid";
		getSingleByGameid(user);
		// 这下面块，到VIP那里，估计客户端也是没用的
		user["todayFirst"] = Util::isToday(Json::SafeConverToInt32(user["mactivetime"])) ? 1 : 0; // 今天第一次登录

		user["wintimes"]    = atoi(accountInfo["wi"].c_str());
		user["losetimes"]   = atoi(accountInfo["ls"].c_str());
		user["drawtimes"]   = atoi(accountInfo["da"].c_str());
		user["helpnum"]     = atoi(accountInfo["helpnum"].c_str());
		user["feedback"]    = atoi(accountInfo["feedback"].c_str());
		user["ctask"]       = atoi(accountInfo["ctask"].c_str());

        std::string strTmp = StrFormatA("lst%" PRIi32, Json::SafeConverToInt32(user["gameid"]));
		user["lastLoginTime"] = accountInfo["lastLoginTime"] = accountInfo.find(strTmp) != accountInfo.end() ? accountInfo[strTmp] : accountInfo["lastLoginTime"];
		std::string continuousLoginDay = accountInfo["continuousLoginDay"];
		//$aUser['loginRewardFlag'] = Logs::factory()->limitCount($aUser['mid'], 7, 0, false) ? 0 : 1;
		//user["loginRewardFlag"] = ;
		if (Util::isToday(Json::SafeConverToInt32(user["lastLoginTime"])))
		{
			int continuousLoginDay = atoi(accountInfo["continuousLoginDay"].c_str());
			user["continuousLoginDay"] = continuousLoginDay + 1;
		}
		else
		{
			user["continuousLoginDay"] = 1;
		}

        string productid = user.isMember("productid") ? user["productid"].asString() : "";
		productid = TrimStr(productid);
        time_t mactivetime = 0;
		try
		{
			mactivetime = user["aMactivetime"].isMember(productid) ? Json::SafeConverToInt32(user["aMactivetime"][productid]) : 0;
		}
		catch (...)
		{
			mactivetime = 0;
		}
		LOGGER(E_LOG_INFO) << "isToday";
		if (!Util::isToday(mactivetime)) // 第一次登录
		{
			doTodayFirst(user);
		}

		user["todayReward"]     = getLoginReward(Json::SafeConverToInt32(user["continuousLoginDay"]));
		user["tomorrowReward"]  = getLoginReward(Json::SafeConverToInt32(user["continuousLoginDay"]) + 1);
		user["vip"]             = 0; //现在没有会员
		///////////////////////////////////////////////////////////

		user["mw"]              = atoi(accountInfo["mw"].c_str());
		user["vipexptime"]      = 0;
		user["bank"]            = user["bankPWD"].asString().empty() ? 0 : 1;
		user["horn"]            = atoi(accountInfo["horn"].c_str());
		user["vipconfig"]["money"]      = 500;
		user["vipconfig"]["vipexptime"] = 30;
		user["prop"]            = 0;
		user["propexptime"]     = 0;
		user["tomorrowChanges"] = 0;
		user["probability"]     = "";
		user["lp1"]             = 0;
		user["lp2"]             = 0;
		user["level"]           = 0;

		std::string dev_no = user["device_no"].asString();
		std::string client_ip = user["ip"].asString();
		user["mtkey"]           = setMtkey(mid ,  dev_no, client_ip);
    
		LOGGER(E_LOG_INFO) << "getHallConf";
		Json::Value conf = GetHallConf(Json::SafeConverToInt32(user["productid"]));
		if (conf.isNull())
		{
			conf = GetHallConf(0);
		}
		user["serverInfo"]["hallAddr"] = conf;
		/*
		Json::Value hall_addrs;
		std::vector<ip_port_conf>* hall_infos = CConfig::getInstance().getHallConf(Json::SafeConverToInt32(user["productid"]));
		if (hall_infos)
		{
			std::vector<ip_port_conf>::iterator it = hall_infos->begin();
			Json::Value conf;
			for (; it != hall_infos->end(); ++it)
			{
				conf["ip"] = it->ip;
				conf["port"] = it->port;

				//hall_addrs.append(conf);
            }
            user["serverInfo"]["hallAddr"] = conf;
		}*/
    
		//LOGGER(E_LOG_INFO) << user.toStyledString();
		Json::Value hall_addrs_bak;
		std::vector<ip_port_conf>* hall_infos_bak = CConfig::getInstance().getHallConfBak(Json::SafeConverToInt32(user["productid"]));
		if (hall_infos_bak)
		{
			std::vector<ip_port_conf>::iterator it2 = hall_infos_bak->begin();
			Json::Value conf2;
			for (; it2 != hall_infos_bak->end(); ++it2)
			{
				conf2["ip"] = it2->ip;
				conf2["port"] = it2->port;

				hall_addrs_bak = conf2;
				break;
				//hall_addrs_bak.append(conf2);
			}
		}
		user["serverInfo"]["hallAddrBackup"] = hall_addrs_bak;

		Json::Value hall_addrs_fish;
		std::vector<ip_port_conf>* hall_fish = CConfig::getInstance().getHallConfFish();
		if (hall_fish)
		{
			std::vector<ip_port_conf>::iterator it3 = hall_fish->begin();
			Json::Value conf3;
			for (; it3 != hall_fish->end(); ++it3)
			{
				conf3["ip"] = it3->ip;
				conf3["port"] = it3->port;

				//hall_addrs_fish.append(conf3);
				hall_addrs_fish = conf3;
			}
		}
		user["serverInfo"]["fishAddr"] = hall_addrs_fish;

		//user["yuleInfo"] = "";
		user["bankruptcy"] = 1000;//破产条件 
		user["bingReward"] = 0;//绑定账号奖励
		user["bankConfig"] = 0;//百家乐转账配置
    
		LOGGER(E_LOG_INFO) << "SafeConverToInt32";
		if (Json::SafeConverToInt32(user["sex"]) == 0)
		{
			user["sex"] = rand() % 2 + 1;
		}
		else
		{
			user["sex"] = Json::SafeConverToInt32(user["sex"]);	//把string类型转成int类型
		}
        
        std::string userinfokey = StrFormatA("UCNF%d", mid);
        std::string iconid;
        redis_minfo->HGET(userinfokey.c_str(), "iconid", iconid);
		//char icon_path[256] = { 0 };
		std::string icon_path;
		std::string iconDomain = CConfig::getInstance().iconDomain;
		if (Json::SafeConverToInt32(user["iconid"]) == 1)
		{
			icon_path = StrFormatA("%s/date/icon/default/%s.png", iconDomain.c_str(), user["iconid"].asString().c_str());
			user["icon"] = icon_path;
			user["big"] = user["icon"];
			user["middle"] = user["icon"];
		}
		else
		{
            std::string ui = user["icon"].asString();
            if (ui == "\"\"") ui.clear();
            if (!ui.empty())
            {
                icon_path = StrFormatA("%s%s", iconDomain.c_str(), ui.c_str());
            }
            else
            {
                icon_path = iconDomain;
            }
			//icon_path = StrFormatA("%s/data/icon/%d/%d_icon.jpg?v=%s", iconDomain.c_str() , mid % 10000 , mid , accountInfo["iconVersion"].c_str());
			user["icon"] = icon_path;
			//std::string middle_path;
			//std::string big_path;
			//middle_path = StrFormatA("%s/data/icon/%d/%d_middle.jpg?v=%s", iconDomain.c_str() , mid % 10000 , mid , accountInfo["iconVersion"].c_str());
			//big_path = StrFormatA("%s/data/icon/%d/%d_big.jpg?v=%s", iconDomain.c_str() , mid % 10000 , mid , accountInfo["iconVersion"].c_str());
			user["middle"] = icon_path;
			user["big"] = icon_path;
		}

		user["pmode"] = 1;
		if (user["gameid"] == 3)
		{
			user["enterRoomID"] = 1;
		}

		user["mmp"] = 1;

		std::string shop_key = "VERKEY";
		std::map< std::string , std::string > verkeys;
		redis_common->HGetAll(shop_key.c_str() , verkeys);
		user["vershop"] = atoi(verkeys["vershop"].c_str());
		user["verwm"] = atoi(verkeys["verwm"].c_str());

		user["ucenterUrl"] = CConfig::getInstance().ucenterUrl;
		user["paycenterUrl"] = CConfig::getInstance().paycenterUrl;

		std::string s_value1 = user["productid"].asString();
		if (!redis_minfo->HSET(userinfokey.c_str() , "productid" , s_value1.c_str()))
		{
			LOGGER(E_LOG_ERROR) << "hset failed";
			return -1;
		}

		std::string c_ip = user["ip"].asString();
		vector<string> ip_arr = Lib_Ip::find(c_ip.c_str());
    
		LOGGER(E_LOG_INFO) << "ip_arr";
		if (ip_arr.empty() || ip_arr[0] != "中国")
		{
			std::string province_f = StrFormatA("%d", mid);
			std::string province = "";
			if (!redis_ucenter->HGET("ip_arr" , province_f.c_str() , province))
			{
				int32_t r_index = GetRandInt(0 , province_ch.size() - 1);
				province = province_ch[r_index];
				redis_ucenter->HSET("ip_arr" , province_f.c_str() , province.c_str());
			}
			ip_arr.push_back(province);
		}
		//LOGGER(E_LOG_INFO) << "jArr";

		Json::Value jArr;
		vector<string>::iterator ipit = ip_arr.begin();
		for (; ipit != ip_arr.end(); ++ipit)
		{
			//Json::Value jsonIp;
			//jsonIp["ip_arr"] = *ipit;
			jArr.append(*ipit);
		}
		user["ip_arr"] = jArr;

		SqlLogic::setLoginRecord(user);

		unsetUserFiled(user);

		//sitemid 客户端就是用String解析的。这个要注意
		std::string strIntKey[] = {"iconid","binding","upmnick",""};	//把string类型转成int类型
		int iKeyIndex = 0;
		while (!strIntKey[iKeyIndex].empty())
		{
			user[strIntKey[iKeyIndex]] = Json::SafeConverToInt32(user[strIntKey[iKeyIndex]]);
			iKeyIndex++;
		}

		return 0;
	}

    /**
    * 给用户加钱/扣钱
    *
    * @param {int}  $mid    用户ID
    * @param {int}  $mode   途径标识  PS：一定要注意这个ID，加钱前请查询后台配置
    * @param {int}  $flag   加钱/扣钱 (0加，1扣)
    * @param {int}  $money  钱数
    * @param {str}  $desc   附加信息
    *
    * @return boolean 成功返回TRUE,否则返回FALSE
    **/

    bool addWin(const Json::Value& info, int money, int mode, int wflag, const char* desc_)
    {
        string desc = ThreadResource::getInstance().getDBConnMgr()->DBMaster()->EscapeString(desc_);
        int gameid = Json::SafeConverToInt32(info["gameid"]);
        int mid = Json::SafeConverToInt32(info["mid"]);
        int sid = Json::SafeConverToInt32(info["sid"]);
        int cid = Json::SafeConverToInt32(info["cid"]);
        int pid = Json::SafeConverToInt32(info["pid"]);
        int ctype = Json::SafeConverToInt32(info["ctype"]);
        if ( !mid || !money ) {
            LOGGER(E_LOG_DEBUG) << "参数为空" << "pay/addwin";
            return false;
        }

        /*
        if(in_array($mode, array(25,28))){
            $_stime = microtime(true);
        }

        if(in_array($mode, array(15,34,31,32,19))){//活动中心，乐宝，翻翻乐加减金币时要判断用户是否在其它游戏玩
            $serverInfo = Loader_Redis::userServer($mid)->hMget(Config_Keys::userServer($mid), array("svid","tid","level"));
            if($serverInfo["svid"] !=0 || $serverInfo["tid"] !=0  ){
                $this->debug(array("mid"=>$mid,"mode"=>$mode,"svid"=>$serverInfo["svid"],"tid"=>$serverInfo["tid"]),"svid_error");
                return false;
            }
        }

        //防沉迷，超过5小时在玩不加金币
        if(Loader_Redis::server()->get(Config_Keys::swicthVerify(),false,false) == 1){
            $playTime = Loader_Redis::server()->hGet(Config_Keys::onlinetimehash(), $mid);
            if(($playTime/3600) > 5){
                //return false;
            }
        }*/

        money = (wflag == 0 ? money : -1*money);
        //$wflag == 0 &&  $money = min(Config_Money::$max , $money );
        Json::Value jvMoneyRet;
        bool bMoneySvrRet = CTcpMsg::setMoney(jvMoneyRet,mid,money);//通知server加钱
        if (!bMoneySvrRet)
        {
            //失败的情况再加一加一次，防止server数据没初始化
            ULOGGER(E_LOG_DEBUG, mid) << "flag=" << bMoneySvrRet << "pay/addwin"<<" , first failed , try again to add money:"<<money;
            bMoneySvrRet = CTcpMsg::setMoney(jvMoneyRet,mid,money);
        }

        ULOGGER(E_LOG_DEBUG, mid) << "flag=" << bMoneySvrRet << "pay/addwin"<<" , add money:"<<money;
        if (bMoneySvrRet) 
	    {
            Json::Value info;
            bool ret = SqlLogic::GetUserInfo(mid, info);
            //int productid = info["productid"].asInt();
            int productid = Json::SafeConverToInt32(info["productid"]);
            string table = productid % 10 == 0 ? "logchip.log_win10" : StrFormatA("logchip.log_win%d", productid % 10);
            desc = desc.length() == 0 ? "C++绑定金币变化" : desc;
            int m = abs(money);
            string sql=StrFormatA("INSERT INTO %s SET mid='%d',gameid='%d',sid='%d',cid='%d',"
                "pid=%d,ctype='%d',wmode=%d,wflag=%d,money=%d,wtime=%d,wdesc='%s'", table.c_str(), mid, gameid, sid,
            cid, pid, ctype, mode, wflag, money, time(NULL), desc.c_str());

            ThreadResource::getInstance().getDBConnMgr()->DBMaster()->ExeSQL((char*)sql.c_str());
        }

        return (bool)bMoneySvrRet;
    }


	bool addNewWin(const std::string &dealNo, int gameid, int mid, int mode, int sid, int pid, int ctype, int wflag, int money, const std::string& desc, int type)
	{
		if (dealNo.empty() || money == 0)
		{
			LOGGER(E_LOG_ERROR) << "参数为空";
			return false;
		}
		money = wflag == 0 ? -money : money;
		// save to redis
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (redisMgr == NULL)
		{
			LOGGER(E_LOG_ERROR) << "getRedisConnMgr is null, it did not be initted when server starting!";
			return false;
		}
		RedisAccess* redis_money = redisMgr->money();
		if (!redis_money)
		{
			LOGGER(E_LOG_ERROR) << "minfo or common redis is null, it did not be initted when server starting!";
			return "";
		}

		std::string key = StrFormatA("MONNEW:%s", dealNo.c_str());
		std::map<std::string, std::string> kv;
		kv["pdealno"] = dealNo;
		kv["mid"] = StrFormatA("%d", mid);
		kv["status"] = StrFormatA("%d", 1);
		kv["money"] = StrFormatA("%d", money);
		kv["type"] = StrFormatA("%d", type);
		redis_money->HMSET(key.c_str(), kv);
		// notify server
		if (!CTcpMsg::setNewMoney(dealNo, mid))
		{
			LOGGER(E_LOG_ERROR) << "notify money server set new money failed";
			return false;
		}
        ULOGGER(E_LOG_INFO , mid) << "new add money, dealNo:" << dealNo;
		//std::string value;
		//redis_money->HGET(key.c_str(), "status", value);
		//if (value == "1")
		//{
			SqlLogic::setWinLog(mid, mode, wflag, money, desc);
		//}
		return true;
	}
    
    int getRoomConfigKey(int gameid, std::string& key, std::string& many)
    {
		CConfig::getInstance().getRoomConfigKey(gameid, key, many);
        return 0;
    }
    
	bool MaintenanceCheck( const char* szClientIp )
	{
		CConfig& conf = CConfig::getInstance();
		if (conf.m_nMaintenanceType == 0) return true;				// 允许登录

		if (conf.m_strMaintenanceCountry.empty()) return false;	// 谁也不能登录了

		int is_ok_ip     = 0;
		vector<string> ip_arr = Lib_Ip::find(szClientIp);

		if(ip_arr.size() > 0)
		{
			LOGGER(E_LOG_INFO) << szClientIp << " you country is:" << ip_arr[0];
			if (conf.m_strMaintenanceCountry == ip_arr[0])
			{
				is_ok_ip = 1;
			}
		}
		if (is_ok_ip == 0) 
		{
			LOGGER(E_LOG_INFO) << szClientIp << " is not allow";
			return false;	// 不允许登录
		}
		return true;	// 允许
	}

	bool IsFastCtrlMoney( int32_t nMid )
	{
		RedisAccess* moneyRedis = GetRdMoney();
		if (NULL == moneyRedis) return true;
		std::string strValue;
		std::string strKey = StrFormatA("FCM|%" PRIi32, nMid);
		if(moneyRedis->GET(strKey.c_str(),strValue)) return true;

		strValue = "1";
		moneyRedis->SET(strKey.c_str(),strValue.c_str(),1);
		return false;
	}

	int getProductRoomConfig(int productid, int version, Json::Value& out)
    {
        Json::Value ret;
        Json::FastWriter jWriter;
        RedisAccess* commRedis = ThreadResource::getInstance().getRedisConnMgr()->common("slave");
		std::string strKey = StrFormatA("%d_%d", productid, version);
        redisReply* roomconfig = commRedis->CommandV("hget CPP|productRoomConfig %s", strKey.c_str());	//key多加了个CPP| 防止跟PHP冲突
        if (!roomconfig || roomconfig->type == REDIS_REPLY_NIL) 
		{
            const char *key;
			const char *many;
			int nIndex = 0;
			std::string strIndex;
            const std::string strTb_Product_gamesetting = "game_route.product_gamesetting";
            string sql = StrFormatA("select * from %s where productid=%d and version=%d order by sort desc,gameid asc",
                                    strTb_Product_gamesetting.c_str(), productid, version);
            MySqlDBAccess* dbRouting = ThreadResource::getInstance().getDBConnMgr()->DBRouting();
            MySqlResultSet* res = dbRouting->Query(sql.c_str());
            if (res && res->getRowNum() > 0) 
			{
                for (; res->hasNext();) 
				{
                    std::string rk, rm;
                    getRoomConfigKey(res->getIntValue("gameid"), rk, rm);
                    key = rk.c_str();
                    many = rm.c_str();
                    int level = res->getIntValue("level");
                    int gameid = res->getIntValue("gameid");
                    RedisAccess* serverRedis = ThreadResource::getInstance().getRedisConnMgr()->server();
                    const char * field_name = "minmoney maxmoney ante tax maxlimit maxallin magiccoin";
                    vector<string> fields = Helper::explode(field_name, " ");
                    map<string, string> redisRet;
                    bool retCode = false;
                    if (level == 5)
                        retCode = serverRedis->HMGET(many, fields, redisRet);
                    else
                        retCode = serverRedis->HMGET(StrFormatA("%s:%d", key, level).c_str(), fields, redisRet);
                    if (retCode) 
					{
						Json::Value conn;
						strIndex = StrFormatA("%" PRIi32,nIndex++);
                        if (productid == 1 && version == 5) 
						{
                            conn["roomid"] = level;
                            for(int j = 0; j < fields.size(); j++) {
                                if (redisRet.find(fields[j]) != redisRet.end())
                                    conn[fields[j]] = atoi(redisRet[fields[j]].c_str());
                            }
                            out[res->getCharValue("gameid")][strIndex] = conn;
                        } 
						else 
						{
                            conn["roomid"] = level;
                            conn["gameid"] = gameid;
                            for(int j = 0; j < fields.size(); j++) {
                                if (redisRet.find(fields[j]) != redisRet.end())
                                    conn[fields[j]] = atoi(redisRet[fields[j]].c_str());
                            }
                            //ret[res->getCharValue("labby")][strIndex] = conn;
							ret[res->getCharValue("labby")].append(conn);
                        }
                    }
                }
            }
            if(res) delete res;
            if (productid != 1 || version != 5) 
			{
				nIndex = 0;
                Json::Value::Members mem = ret.getMemberNames();
                for (Json::Value::Members::iterator iter = mem.begin(); iter != mem.end(); iter++) 
				{
					strIndex = StrFormatA("%" PRIi32,nIndex++);
                    Json::Value tmp;
                    tmp["name"] = *iter;
                    tmp["roomconfig"] = ret[*iter];
					out[strIndex] = tmp;
					
                }
            }
			if (!out.isNull())
			{
				commRedis->HSET("CPP|productRoomConfig", StrFormatA("%d_%d", productid, version).c_str(), jWriter.write(out).c_str());	//key多加了个CPP| 防止跟PHP冲突
			}
        } 
		else 
		{
            Json::Reader jReader;
            jReader.parse(roomconfig->str, out);
        }
        if (roomconfig) freeReplyObject(roomconfig);
        
        //LOGGER(E_LOG_DEBUG) << out.toStyledString();
        
        return 0;
    }
    
    void getServerInfo(int gameid, int product, Json::Value& out)
    {
        Json::Value hall_addrs;
        std::vector<ip_port_conf>* hall_infos = CConfig::getInstance().getHallConf(product);
        if (hall_infos)
        {
            std::vector<ip_port_conf>::iterator it = hall_infos->begin();
            Json::Value conf;
            for (; it != hall_infos->end(); ++it)
            {
                conf["ip"] = it->ip;
                conf["port"] = it->port;
                break; //取第一个
            }
            out["serverInfo"]["hallAddr"] = conf;
        }
        
        RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
        RedisAccess* game = redisMgr->game();
        std::string key, many;
        Json::Value cfg;
        for (int i = 1; i <= 5; i++)
        {
            getRoomConfigKey(gameid, key, many);
            std::map<std::string, std::string> mp;
            key = StrFormatA("%s%d", key.c_str(), i);
            game->HGetAll(key.c_str(), mp);
            if (mp.empty())
            {
                LOGGER(E_LOG_WARNING) << "no room config :" << key;
                continue;
            }
            cfg["roomid"] = i;
            cfg["minmoney"] = atoi(mp["minmoney"].c_str());
            cfg["maxmoney"] = atoi(mp["maxmoney"].c_str());
            cfg["ante"] = atoi(mp["ante"].c_str());
            cfg["tax"] = atoi(mp["tax"].c_str());
            cfg["maxlimit"] = atoi(mp["maxlimit"].c_str());
            cfg["maxallin"] = atoi(mp["maxallin"].c_str());
            cfg["rase1"] = atoi(mp["rase1"].c_str());
            cfg["rase2"] = atoi(mp["rase2"].c_str());
            cfg["rase3"] = atoi(mp["rase3"].c_str());
            cfg["rase4"] = atoi(mp["rase4"].c_str());
            cfg["magiccoin"] = atoi(mp["magiccoin"].c_str());
            out["serverInfo"]["roomconfig"].append(cfg);
        }
    }
    
    void getYuleInfo(Json::Value& out)
    {
        RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
        RedisAccess* game = redisMgr->game();
        //水果机
        std::map<std::string, std::string> fruitRet;
        game->HGetAll("Fruit_RoomConfig", fruitRet);
        Json::Value& info = out["yuleInfo"];
        info["yuleopen"] = 1;
        if (!fruitRet.empty())
        {
            Json::Value fv;
            fv["name"] = "fruit";
			info["yuleconfig"].append(fv);
        }
    }

	bool SetAuthKey( int64_t nMid, const std::string& strAuthKey )
	{
		if (0 == nMid || strAuthKey.empty()) return false;

		RedisAccess* rdUser = GetRdAccount();
		if (NULL == rdUser)
		{
			LOGGER(E_LOG_WARNING) << "Account Redis Is Null";
			return false;
		}
		std::string strKey = StrFormatA("%" PRIu64, nMid);
		std::string strValue;
		if(!rdUser->HSET("AucthKey",strKey.c_str(),strAuthKey.c_str()))
		{
			LOGGER(E_LOG_WARNING) << "Account Redis HSET(AucthKey, " << nMid << ") Is Error .";
			return false;
		}
		return true;
	}

	void getOfficialWechat(Json::Value& out)
	{
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		RedisAccess* game = redisMgr->game();
		std::map<std::string, std::string> wechatRet;
		if (!game->HGetAll("WechatInfo", wechatRet))
		{
			LOGGER(E_LOG_ERROR) << "get wechat info failed!";
			return;
		}
		Json::Value& wechat = out["WechatInfo"];
		for (auto it = wechatRet.begin(); it != wechatRet.end(); it++)
		{
			wechat[it->first] = it->second;
		}
		LOGGER(E_LOG_INFO) << wechat.toStyledString();
	}

	static const std::string roomcardhash = "UserRoomCard";
	void getUserRoomCardInfo(Json::Value& out)
	{
		int uid = Json::SafeConverToInt32(out["mid"]);
		if (uid == 0)
		{
			LOGGER(E_LOG_ERROR) << "uid == 0!";
			return;
		}
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		RedisAccess* ote = redisMgr->ote(uid);
		Json::Value& cardinfo = out["RoomCard"];
		for (int i = 0; i < 4; i++)
		{
			int type = i + 1;
			std::string hash = StrFormatA("%s:%d:%d", roomcardhash.c_str(), uid, type);
			Json::Value info;
			info["type"] = type;
			auto reply = ote->CommandV("hget %s %s", hash.c_str(), "count");
			if (reply)
			{
				if (reply->type == REDIS_REPLY_STRING)
				{
					int count = atoi(reply->str);
					info["count"] = count;
				}
				freeReplyObject(reply);
			}
			else
			{
				info["count"] = 0;
			}
			cardinfo.append(info);
			
		}
	}
}
