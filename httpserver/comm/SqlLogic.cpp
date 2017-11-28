#include "SqlLogic.h"
#include "threadres.h"
#include "StrFunc.h"
#include "RedisLogic.h"
#include <inttypes.h>
#include <map>
#include "md5.h"
#include "Helper.h"
#include "TcpMsg.h"
#include <mutex>
#include "JsonValueConvert.h"
#include "Util.h"
#include "Lib_Ip.h"

namespace SqlLogic
{
//////////////////////////////////////////////////////////////////////////
#define GetDbConn(dbConn,ret,dbFunc)	\
	MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();	\
	if (!dbMgr)	\
	{   \
		LOGGER(E_LOG_ERROR) << "getDBConnMgr is null, it did not be initted when server starting!";	\
		return ret;	\
	}	\
	MySqlDBAccess* dbConn = dbMgr->dbFunc();	\
	if (NULL == dbConn)	\
	{   \
		LOGGER(E_LOG_ERROR) << "MySqlDBAccess is null, it did not be initted when server starting!";	\
		return ret;	\
	}
#define GetDbMaster(dbConn,ret) GetDbConn(dbConn,ret,DBMaster)
#define GetDbSlave(dbConn,ret) GetDbConn(dbConn,ret,DBSlave)
#define GetDbRouting(dbConn,ret) GetDbConn(dbConn,ret,DBRouting)
//////////////////////////////////////////////////////////////////////////

	//std::mutex mtx;

	const std::string db_game_allpay = "game_allpay";

	const std::string db_ucenter                 = "ucenter";
	const std::string db_paycenter               = "paycenter";
	const std::string db_stat                    = "stat";
	const std::string db_statcenter              = "statcenter";
	const std::string db_zq_admin                = "zq_admin";
	const std::string db_exchange                = "exchange";
	const std::string db_behavior                = "behavior";
	const std::string db_logchip = "logchip";
	const std::string db_gameroute = "game_route";

	const std::string strTB_Aliuser              = "ucenter.uc_aliuser";
	const std::string strTb_UserInfo             = "ucenter.uc_userinfo";
	const std::string strTb_GameInfo             = "ucenter.uc_gameinfo";
	const std::string strTb_SitemId2Mid          = "ucenter.uc_sitemid2mid";
	const std::string strTb_Android_Register     = "ucenter.uc_register_android";
	const std::string strTb_Ios_Register         = "ucenter.uc_register_ios";
	const std::string strTb_Username_Register    = "ucenter.uc_register_username";
	const std::string strTb_Phonenumber_Register = "ucenter.uc_register_phonenumber";
	const std::string strTb_Register_Sitemid     = "ucenter.uc_register_sitemid";
	const std::string strTb_Account_Binding      = "ucenter.uc_account_binding";
	const std::string strTb_Feedback             = "ucenter.uc_feedback";
	const std::string strTb_Feedback_Reply       = "ucenter.uc_feedback_reply";
	const std::string strTb_Message_Logs         = "ucenter.uc_logmessage";
	const std::string strTb_SettingSid           = "ucenter.uc_setting_sid";
	const std::string strTb_SettingCid           = "ucenter.uc_setting_cid";
	const std::string strTb_SettingPid           = "ucenter.uc_setting_pid";
	const std::string strTb_Notice               = "ucenter.uc_notice";
	const std::string strTb_Versions             = "ucenter.uc_versions";
	const std::string strTb_StatSum              = "stat.stat_sum";
	const std::string strTb_Setting_Pmode        = "ucenter.uc_setting_pmode";
	const std::string strTb_Paymentid            = "ucenter.uc_paymentid";
	const std::string strTb_Payment              = "ucenter.uc_payment";
	const std::string strTb_LogMember            = "logchip.log_member";
	const std::string strTb_LogFish              = "logchip.log_member_fish";
	const std::string strTb_LogRoll              = "logchip.log_roll";
	const std::string strTb_LogBank              = "logchip.log_bank";
	const std::string strTb_LogRonline           = "stat.stat_logonline";
	const std::string strTb_WinLog               = "logchip.log_win";
	const std::string strTb_Wmode                = "ucenter.uc_setting_wmode";
	const std::string strTb_Antiaddiction        = "ucenter.uc_antiaddiction";
	const std::string strTb_PushConfig           = "ucenter.uc_setting_push";
	const std::string strTb_Match_Prize          = "ucenter.uc_match_prize";
	const std::string strTb_LogActive            = "logchip.log_active";
	const std::string strTb_LogRegister          = "logchip.log_register";
	const std::string strTb_LogCalregister       = "logchip.log_calregister";
	const std::string strTb_LogBataccount        = "ucenter.uc_logbataccount";
	const std::string strTb_SystemRecharge       = "ucenter.uc_system_recharge";
	const std::string strTb_StatRegister         = "statcenter.stat_register";
	const std::string strTb_wx_register          = "ucenter.uc_register_wx";
	const std::string strTb_withdrawals          = "paycenter.pc_withdrawals";
	const std::string strTb_pc_payment           = "paycenter.pc_payment";
	const std::string strTb_admin_versions       = "zq_admin.product_version";
	const std::string strTb_forbid               = "zq_admin.forbid";
	const std::string strTb_SystemRechargeOther  = "ucenter.uc_system_recharge_other";

	int specialAccountArray[] = {4040798,4179248,4039929,4436684,4436707,4436725,4436743,4413006,4412265,714567,4508537,4531782,4531822,4321983,5083739,5241908};
	set<int> specialAccount(specialAccountArray, specialAccountArray + sizeof(specialAccountArray)/sizeof(int));

	bool GetUserInfo(int32_t nMid, Json::Value& info, bool bUseCache /*= true*/)
	{
		//查询用户信息及充值记录
		GetDbMaster(sqlDb, false);
		bool bRedisHaveDate = false;
		if (bUseCache)
		{
			if(!RedisLogic::GetUserInfo(nMid,info)) bUseCache = false;
		}

		if (!bUseCache)   // redis能够获取数据
		{
			//redis拿不到数据，需要从mysql获取
			std::string query = StrFormatA("SELECT * FROM ucenter.uc_userinfo%d WHERE mid = %d LIMIT 1;", nMid%10, nMid);
			auto result = sqlDb->Query(query.c_str());
			if (result == NULL)
			{
				LOGGER(E_LOG_ERROR) << "get userinfo failed!";
				return false;
			}
			std::string strMActiveTime,strMEnterCount,strMTime;
			Json::Value jvMActiveTime,jvMEnterCount,jvMTime;
			if (result->hasNext())
			{
				//TODO:考虑改为field获取
				//int colnum = result->getColumnNum();
				info["mid"] = result->getIntValue("mid");
				info["sid"] = result->getIntValue("sid");
				info["cid"] = result->getIntValue("cid");
				info["pid"] = result->getIntValue("pid");
				info["ctype"] = result->getIntValue("ctype");
				info["mnick"] = result->getCharValue("mnick");
				info["sitemid"] = result->getCharValue("sitemid");
				info["sex"] = result->getIntValue("sex");
				info["hometown"] = result->getCharValue("hometown");
				info["mstatus"] = result->getIntValue("mstatus");
				info["ip"] = Helper::long2ip(result->getIntValue("ip"));
				info["devicename"] = result->getCharValue("devicename");
				info["osversion"] = result->getCharValue("osversion");
				info["nettype"] = result->getIntValue("nettype");
				info["versions"] = result->getCharValue("versions");
				info["openid"] = result->getCharValue("openid");
				info["productid"] = result->getIntValue("productid");
				info["icon"] = result->getCharValue("icon");
				info["bankPWD"] = result->getCharValue("bankPWD");
				info["ctime"] = result->getInt64Value("ctime");

				info["mactivetime"] = result->getCharValue("mactivetime");
				info["mentercount"] = result->getCharValue("mentercount");
				info["mtime"] = result->getCharValue("mtime");

			}
			if(result) delete result;
			
			info["alipay"] = info["aliname"] = "";
			query = StrFormatA("SELECT alipay,aliname FROM ucenter.uc_aliuser WHERE mid = '%d' LIMIT 1;", nMid);
			result = sqlDb->Query(query.c_str());
			if (result && result->hasNext())
			{
				info["alipay"] = result->getCharValue("alipay");
				info["aliname"] = result->getCharValue("aliname");
				//以前php就写反了，将错就错吧
				//info["alipay"] = result->getCharValue("aliname");
				//info["aliname"] = result->getCharValue("alipay");
			}
			if(result) delete result;

			info["phoneno_binding"] = "";
			info["binding"] = 0;
			if (Json::SafeConverToInt32(info["sitemid"]) > 0)
			{
				query = StrFormatA("SELECT phoneno FROM %s WHERE sitemid=%" PRIi32 " and status=1 LIMIT 1;", 
					strTb_Account_Binding.c_str(), Json::SafeConverToInt32(info["sitemid"]));
				result = sqlDb->Query(query.c_str());
				if (result && result->hasNext())
				{
					info["phoneno_binding"] = result->getCharValue("phoneno");
					info["binding"] = 1;
				}
				if(result) delete result;
			}

			if (info.isNull()) return false;
			return RedisLogic::SetUserInfo(nMid, info, 2 * 24 * 3600); //写入redis 
		}
		else
		{
			//类型转换一下,sitemid 客户端就是用String解析的。这个要注意 
			std::string strIntKey[] = {"mid","sid","cid","pid","ctype","sex","mstatus","ip","nettype","productid","ctime","binding","iconid","binding","upmnick",""};	//把string类型转成int类型
			int iKeyIndex = 0;
			while (!strIntKey[iKeyIndex].empty())
			{
				if(!info[strIntKey[iKeyIndex]].isNull()) 
					info[strIntKey[iKeyIndex]] = Json::SafeConverToInt32(info[strIntKey[iKeyIndex]]);
				iKeyIndex++;
			}
		}
		return true;
		
	}

	bool GetGameInfo(int uid, Json::Value& info)
	{
		bool flag = CTcpMsg::getMoney(info, uid);
		if (!flag)
		{
			return false;
		}
		// roll相关应该不需要，暂不处理
		return true;
	}

	int  GetSiteMidByKey(std::string deviceNo, int productId)
	{
		GetDbMaster(dbMaster,0);

		int siteMid = 0;
		std::string strSql = StrFormatA("SELECT sitemid FROM ucenter.uc_register_android WHERE device_no='%s' and productid=%d LIMIT 1",
			deviceNo.c_str(), productId);
		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		if (result) 
		{
			if (result->hasNext())
			{
				siteMid = result->getIntValue(0);
			}
			delete result;
		}

		if (siteMid == 0) 
		{
			siteMid = CreateSiteMid();
			if (siteMid > 0) 
			{
				strSql = StrFormatA("INSERT INTO ucenter.uc_register_android SET sitemid=%d,device_no='%s',productid='%d'",
					siteMid, deviceNo.c_str(), productId);
				dbMaster->ExeSQLNoRet(strSql.c_str());
			}
		}
		return siteMid;
	}

	int CreateSiteMid() 
	{
		GetDbMaster(dbMaster,0);

		std::string strSql = "INSERT INTO ucenter.uc_register_sitemid (sitemid) Value (0)";

		int ret = dbMaster->ExeSQL(strSql.c_str());
		if (ret < 0) {
			LOGGER(E_LOG_ERROR) << "error query ";
			return 0;
		}
		int sitemid = dbMaster->GetLastInsertId();
		LOGGER(E_LOG_INFO) << "new sitemid =" << sitemid;
		return sitemid;
	}

	/**
    * 通过IOS手机设备号获取sitemid
    * 
    */
	int  getSitemidByIosKey(std::string strDeviceNo, std::string strOpenudId, int nProductId /*= 1*/) 
	{
		GetDbMaster(dbMaster,0);

		int sitemid = 0;
		if(strDeviceNo.empty()) return sitemid;

		std::string strSql;
	
		strDeviceNo =dbMaster ->EscapeString(strDeviceNo);
		strOpenudId = dbMaster->EscapeString(strOpenudId);
		strOpenudId = strOpenudId.empty() ? "000000" : strOpenudId;

		if (strDeviceNo != "00000000-0000-0000-0000-000000000000")
		{
			strSql = StrFormatA("SELECT sitemid FROM %s WHERE (openudid='%s' or device_no='%s') and productid='%" PRIi32 "' LIMIT 1" , 
				strTb_Ios_Register.c_str(), strOpenudId.c_str(), strDeviceNo.c_str(), nProductId);
		}
		else
		{
			strSql = StrFormatA("SELECT sitemid FROM %s WHERE openudid='%s' and productid='%" PRIi32 "' LIMIT 1" , 
				strTb_Ios_Register.c_str(), strOpenudId.c_str(), nProductId);
		}
		MySqlResultSet* result = dbMaster->Query((char*)strSql.c_str());
		if (result)
		{
			if(result->hasNext())
			{
				sitemid = result->getIntValue(0);
			}
			delete result;
		}
	
		if (sitemid <= 0 && (nProductId == 1111 || nProductId == 1113 || nProductId == 1115 || nProductId == 9 || (nProductId <0 && nProductId > 5)))
		{
			std::string strProductsIn;
			if (nProductId == 1111 || nProductId == 1113 || nProductId == 1115)
			{
				strProductsIn = "1111,1113,1115";
			}
			else
			{
				strProductsIn = "1,2,3,4,9";
			}
			if (strDeviceNo != "00000000-0000-0000-0000-000000000000")
			{
				strSql = StrFormatA("SELECT sitemid FROM %s WHERE (openudid='%s' or device_no='%s') and productid in ('%s') LIMIT 1" , 
					strTb_Ios_Register.c_str(), strOpenudId.c_str(), strDeviceNo.c_str(), strProductsIn.c_str());
			}
			else
			{
				strSql = StrFormatA("SELECT sitemid FROM %s WHERE openudid='%s' and productid in ('%s') LIMIT 1" , 
					strTb_Ios_Register.c_str(), strOpenudId.c_str(), strProductsIn.c_str());
			}
			MySqlResultSet* result = dbMaster->Query((char*)strSql.c_str());
			if (result)
			{
				if(result->hasNext())
				{
					sitemid = result->getIntValue(0);
				}
				delete result;
			}
		}
	
		if (sitemid <= 0)
		{
			sitemid = CreateSiteMid();
			if (sitemid > 0)
			{
				strSql = StrFormatA("INSERT INTO %s SET sitemid='%" PRIi32 "',device_no='%s', openudid='%s', productid='%" PRIi32 "';" , 
					strTb_Ios_Register.c_str(), sitemid, strDeviceNo.c_str(), strOpenudId.c_str(), nProductId);
				dbMaster->ExeSQLNoRet((char*)strSql.c_str());
			}
		}
		return sitemid;
	}


    int  getSitemidByMid(int mid)
    {
        GetDbMaster(dbMaster , 0);
        std::string strSql = StrFormatA("select sitemid from ucenter.uc_register_username where mid='%d'limit 1 " , mid );
        MySqlResultSet* result = dbMaster->Query(strSql.c_str());
        if (!result)
        {
            return 0;
        }

        if (!result->hasNext())
        {
            delete result;
            return 0;
        }

        int sitemid = result->getIntValue(0);
        delete result;
        return sitemid;
    }


	bool GetDeviceForbidInfo(std::string& ip, std::string& device_no,std::string& openudid)
	{
		GetDbMaster(dbMaster,false);

		std::string strSql = StrFormatA("select forbid from zq_admin.forbid where (forbid_type=1 or forbid_type=3) and (forbid='%s' or forbid='%s' or forbid = '%s' )",
			ip.c_str(), device_no.c_str(), openudid.c_str() );

		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		if ( !result ) return false;

		//std::string forbid = result->getCharValue()
		delete result;
		return true;
	}

	bool GetDeviceForbidInfoByMid(int32_t nMid)
	{
		//为0 直接通过
		if(nMid == 0)
		{
			return false;
		}

		GetDbMaster(dbMaster,false);

		std::string tableName = StrFormatA("uc_userinfo%d",nMid%10);

		std::string strSql = StrFormatA("select mstatus from ucenter.%s where mid = %d",tableName.c_str(),nMid);

		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		
		//return true 为禁止 false 为不禁止

		if ( !result )
		{
			return true;
		}
		if(!result->hasNext())
		{
			delete result;
			return true;
		}

		int mstatus = result->getIntValue(0);
		delete result;
		//'用户状态 1:禁止進入 8:用戶卸載應用后重裝 9:用戶需要更新資料 10:用戶正常狀態',
		if(mstatus == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int getSitemidByPhoneNumber(std::string& phoneNo, std::string& passwd)
	{
		GetDbMaster(dbMaster,0);

		std::string safe_phoneNo = dbMaster->EscapeString(phoneNo.c_str() , phoneNo.size());
		MD5 md5(passwd);
		std::string password = md5.md5();

		std::string strSql = StrFormatA("select sitemid from %s where phoneno='%s' and password='%s' limit 1 ",
			strTb_Phonenumber_Register.c_str(), safe_phoneNo.c_str(), password.c_str() );	//如果只有select 是不是可以走从库

		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		if ( !result )
		{
			return 0;
		}
		if(!result->hasNext())
		{
			delete result;
			return 0;
		}
		int sitemid = result->getIntValue(0);
		delete result;
		return sitemid;
	}

	int getSitemidByuserName(std::string& userName , std::string& strPsw)
	{
		GetDbMaster(dbMaster,0);

		std::string strUserName = dbMaster->EscapeString(userName);
		std::string strMd5Psw = MD5::CalMd5(strPsw);

		std::string strSql = StrFormatA("select sitemid from %s where username='%s' and (password='%s' or password='%s') limit 1;" ,
			strTb_Username_Register.c_str(), strUserName.c_str() , strMd5Psw.c_str(), strPsw.c_str());  //如果只有select 是不是可以走从库

		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		if (!result)
		{
			LOGGER(E_LOG_ERROR) << "result is null!";

			return 0;
		}
		if(!result->hasNext())
		{
			delete result;
			return 0;
		}
		int sitemid = result->getIntValue(0);
		delete result;
		return sitemid;
	}

	int getAccountByBinding(std::string& phoneNo, std::string& passwd, int nProductId)
	{
		GetDbMaster(dbMaster,0);

		std::string safe_phoneNo = dbMaster->EscapeString(phoneNo.c_str() , phoneNo.size());
		std::string strSql = StrFormatA("select sitemid from %s where phoneno='%s' and status=1 and productid=%d limit 1 ",
			strTb_Account_Binding.c_str(), safe_phoneNo.c_str(), nProductId);
		MySqlResultSet* result = dbMaster->Query( strSql.c_str() );
		if ( !result )
		{
			return 0;
		}
		if(!result->hasNext())
		{
			delete result;
			return 0;
		}
		int sitemid = result->getInt64Value(0);
		delete result;
		if (sitemid == 0)
		{
			return 0;
		}

		MD5 md5(passwd);
		std::string password = md5.md5();
		strSql = StrFormatA("select sitemid from ucenter.uc_register_username where sitemid='%d' and password='%s' limit 1 ",
			sitemid, password.c_str());
		result = dbMaster->Query( strSql.c_str() );
		if ( !result )
		{
			return 0;
		}
		if(!result->hasNext())
		{
			delete result;
			return 0;
		}
		int sitemid2 = result->getIntValue(0);
		assert( sitemid == sitemid2 );

		delete result;
		return sitemid;
	}

	uint32_t doMasterSql(const char* sql)
	{
		GetDbMaster(dbMaster,-1);

		if (sql == NULL) 
		{
			LOGGER(E_LOG_ERROR) << "Sql String Is NULL!";
			return -1;
		}
		return dbMaster->ExeSQL(sql);
	}

	int getOneBySitemid(int32_t sitemid , int32_t sid , Json::Value& user, bool inCache)
	{
		if ( sid <= 0 || sitemid == 0 )
		{
			return -1;
		}

		int mid = sitemid2mid(sitemid , sid);
		if (mid <= 0)
		{
			return -2;
		}

		if (!getOneById(user , mid))
		{
			return -3;
		}

		return 0;
	}

	int sitemid2mid(int32_t sitemid , int32_t sid)
	{
		GetDbMaster(dbMaster,0);

		if (sid <= 0 || sitemid <= 0)
		{
			sid = 0;
		}

		int mid = getSpecialMidBySitemid(sitemid);
		if (mid > 0)
		{
			return mid;
		}
    
		std::string cachekey = getSitemid2MidKey(sitemid , sid);
	  /*  RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if ( redisMgr == NULL )
		{
			LOGGER(E_LOG_ERROR) << "redis mgr of thread did not initted, cache key is:  "<<cachekey;
			return 0;
		}*/
    
		// 先从 redis 查
		mid = RedisLogic::getMidByCacheKey(cachekey);
		if (mid > 0)
		{
			return mid;
		}
		std::string strSql = StrFormatA("SELECT mid FROM %s WHERE sitemid='%d' ORDER BY mid DESC LIMIT 1",  
			strTb_SitemId2Mid.c_str(), sitemid );
		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		if (result == NULL)
		{
    		return 0;
		}
		if(!result->hasNext())
		{
			delete result;
			return 0;
		}
		mid = result->getIntValue(0);
		delete result;
    
		//写入 redis 
		RedisLogic::setMidWithCacheKey(cachekey, mid, false, false);
		return mid;
	}

	std::string getSitemid2MidKey(int32_t sitemid , int32_t sid)
	{
		return StrFormatA("UMID%d|%d" , sitemid , sid);
	}

	int getSpecialMidBySitemid(int sitemid)
	{
		static std::map<int , int> specialSitemid;
		if (specialSitemid.empty())
		{
			specialSitemid[4192] = 11888;
			specialSitemid[4195] = 11666;
			specialSitemid[4196] = 22888;
			specialSitemid[4193] = 22666;
		}

		std::map<int , int>::iterator it = specialSitemid.find(sitemid);
		if (it == specialSitemid.end())
		{
			return 0;
		}

		return it->second;
	}

	/**
    * 根据用户的游戏ID获取用户信息
    * 
    * @author gaifyyang
    * @param  int  $mid      用户游戏ID
    * @return array
    */
	bool getOneById(Json::Value& jvRet, int32_t nMid, bool bUseCache /*= true*/) 
	{
		if (nMid < 0) 
		{
			LOGGER(E_LOG_ERROR) << "Error User Id: "<< nMid;
			return false;
		}

		Json::Value jvRetUserInfo;
		if(!GetUserInfo(nMid, jvRetUserInfo, bUseCache))
		{
			LOGGER(E_LOG_ERROR) << "Get UserInfo Error:"<< nMid;
			return false;
		}
		LOGGER(E_LOG_DEBUG) << jvRetUserInfo.toStyledString();
		Json::Value jvRetGameInfo;
		if(!GetGameInfo(nMid, jvRetGameInfo))
		{
			LOGGER(E_LOG_ERROR) << "Get GameInfo Error:"<< nMid;
			return false;
		}
		LOGGER(E_LOG_DEBUG) << jvRetGameInfo.toStyledString();
		return MergeJson(jvRet, jvRetUserInfo, jvRetGameInfo);
	}

	bool MergeJson(Json::Value& jvRet, const Json::Value& jvMerge1, const Json::Value& jvMerge2) 
	{
		if (jvMerge1.empty() && jvMerge2.empty()) return false;
		jvRet = jvMerge1;
		if (jvMerge2.empty())  return true;

		return MergeJson(jvRet,jvMerge2);
	}

	bool MergeJson(Json::Value& jvRet, const Json::Value& jvMerge) 
	{
		if (jvMerge.empty()) return false;
		Json::Value::Members mem = jvMerge.getMemberNames(); 
		for (Json::Value::Members::iterator itMen = mem.begin(); itMen != mem.end(); itMen++)
		{
			if (jvMerge[*itMen].type() == Json::objectValue)
			{
				MergeJson(jvRet[*itMen],jvMerge[*itMen]);
			}
			else if (jvMerge[*itMen].type() == Json::arrayValue)
			{
				int nArrSize = jvMerge[*itMen].size();                
				for (int i = 0; i < nArrSize; i++)                
				{
					jvRet[*itMen][i] = jvMerge[*itMen][i];
				}
			}
			else
			{
				jvRet[*itMen] = jvMerge[*itMen];
			}
		}

		return true;
	}
/*
	bool SetRegAliPayInfo(int uid, CBindAliPayInfo& info, bool updata)
	{
		if (info.alipay == "" || info.aliname == "")
		{
			return false;
		}

		GetDbMaster(dbMaster,false);

		std::string strSql;
		if (updata)
		{
			strSql = StrFormatA("update %s set mid='%d', gameid='%d', alipay='%s', aliname='%s' where mid='%d'", 
				strTB_Aliuser.c_str(), info.mid,info.gameid,info.alipay.c_str(),info.aliname.c_str(),uid);
		}
		else
		{
			strSql = StrFormatA("insert into %s (mid, gameid, alipay, aliname) values ( '%d', '%d', '%s', '%s') ", 
				strTB_Aliuser.c_str(), info.mid,info.gameid,info.alipay.c_str(),info.aliname.c_str());
		}

		dbMaster->ExeSQLNoRet(strSql.c_str());
		return true;
	}


	bool GetRegAliPayInfo(int uid, CBindAliPayInfo& info)
	{
		GetDbMaster(dbMaster,false);

	//	std::lock_guard<std::mutex> lck(mtx);
		std::string strSql = StrFormatA("select gameid, alipay, aliname from %s where mid = %d", strTB_Aliuser.c_str(), uid);
		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		if (!result) return false;
		if(!result->hasNext()) 
		{
			delete result;
			return false;
		}

		info.mid = uid;
		info.gameid = result->getIntValue(0);
		info.alipay = result->getCharValue(1);
		info.aliname = result->getCharValue(2);

		delete result;

		return true;
	}*/

	//************************************
	// Method:    getBankPWD 	根据mid获取保险箱密码
	// FullName:  MySqlDBAccessMgr::getBankPWD
	// Access:    public 
	// Returns:   std::string
	// Qualifier:
	// Parameter: int32_t nMid	用户信息表的ID
	//************************************
	std::string getBankPWD( int32_t nMid )
	{
		GetDbMaster(dbMaster,"");

		std::string strTableName = getTableName(nMid,strTb_UserInfo,10);
		std::string strQuery = StrFormatA("SELECT IFNULL(bankPWD,\"\") as bankPWD FROM %s WHERE mid = '%" PRIi32 "';", strTableName.c_str(), nMid);

		MySqlResultSet* result = dbMaster->Query((char*)strQuery.c_str());
		if (!result) return "";
		if(!result->hasNext()) 
		{
			delete result;
			return "";
		}
		std::string strRet = result->getCharValue(0);
		delete result;
		return strRet;
	}

	//************************************
	// Method:    setUserInfo	更新用户信息
	// FullName:  MySqlDBAccessMgr::setUserInfo
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: int32_t uMid			用户id
	// Parameter: std::map<std::string, std::string> mpInsert	要修改的内容
	//************************************
	bool setUserInfo( int32_t nMid, std::map<std::string,std::string> mpInsert)
	{
		if (mpInsert.size() == 0) return false;

		GetDbMaster(dbMaster,false);

		if (mpInsert.find("mnick") != mpInsert.end())	//昵称， 需要过滤一下注入
		{
			mpInsert["mnick"] = TrimStr(mpInsert["mnick"]);
			mpInsert["mnick"] = dbMaster->EscapeString(mpInsert["mnick"].c_str(),mpInsert["mnick"].length());
		}

		//std::string strCacheKey = CConfigKeys::getUserInfo(uMid);
		//RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		//if (redisMgr == NULL) return false;

		Json::Value info;
		if (!RedisLogic::GetUserInfo(nMid, info)) return false;

		static std::set<string> ssAllow;	//允许更改的字段
		if (ssAllow.size() == 0)
		{
			ssAllow.insert("mnick");
			ssAllow.insert("sex");
			ssAllow.insert("hometown");
			ssAllow.insert("bankPWD");
			ssAllow.insert("upmnick");
			ssAllow.insert("icon");
		}

		std::string strUpdateValue = "";
		for (std::map<std::string,std::string>::iterator itTmp = mpInsert.begin() ; itTmp != mpInsert.end() ; itTmp ++)
		{
			if (itTmp->second.empty()) continue;

			if (ssAllow.end() != ssAllow.find(itTmp->first))
			{
				if (!strUpdateValue.empty())
				{
					strUpdateValue += ",";
				}
				strUpdateValue += StrFormatA("`%s`='%s'",itTmp->first.c_str(), itTmp->second.c_str());
				info[itTmp->first] = itTmp->second;
			}
		}
		if (strUpdateValue.empty()) return false;
		std::string strTableName = getTableName(nMid, strTb_UserInfo, 10);
		std::string strQuery = StrFormatA("UPDATE %s SET %s WHERE mid=\'%" PRIi32 "\' LIMIT 1",strTableName.c_str(),strUpdateValue.c_str(),nMid);
		int nRowCount = dbMaster->ExeSQL((char*)strQuery.c_str());
		if (nRowCount <= 0) return false;

		int uid = Json::SafeConverToInt32(info["mid"]);
		if (!info["mid"].isNull() && nMid == uid) RedisLogic::SetUserInfo(nMid, info, 2*24*3600);
	
		return true;
	}

	/**
		* 每天的次数(累加器)限制.比如每天领奖的次数 
		* @param int $mid 用户ID
		* @param int $ltype 计数器类型: 10发feed ...请调用者加到这里
		* @param int $lcount 要加的数量. 如果是0则只获取
		* @param bool $update 是否要加加,默认为true,有时候只想获得当前的数量
		* @return int 当前的最新值.注意如果没有正确取到缓存则返回一个最大值(如果从来没设置过也是)
		*/
	int64_t limitCount( std::string strKey, std::string strTtype, int64_t nCount, bool bUpdate /*= true*/,int32_t nExpire /*= 86400*/) 
	{
		if(strKey.empty() || nCount < 0) return -1;
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		if (NULL == redisMgr) return -1;
		RedisAccess* redisCommon = redisMgr->common();
		if (NULL == redisCommon) return -1;

		std::string strCacheKey = StrFormatA("ULT|%s|%s", strKey.c_str(), strTtype.c_str());

		if (strTtype == "reg" || strTtype == "ip_reg" || strTtype == "uuid") 
		{
			strCacheKey += "|" + Util::formatGMTTime_Date();
		}
		std::string strValue;
		bool bOk = redisCommon->GET(strCacheKey.c_str(),strValue);
		if(bUpdate)
		{
			return redisCommon->INCRBY(strCacheKey.c_str(),nCount,nExpire);
		}
		else if(bOk)
		{
			return atoi(strValue.c_str());
		}
		return -1;
	}	

	//************************************
	// Method:    getTableName	取得分表的表名
	// FullName:  MySqlDBAccessMgr::getTableName
	// Access:    public 
	// Returns:   std::string	返回分表的名称
	// Qualifier:
	// Parameter: int32_t uTbid	
	// Parameter: std::string strTableName	分表的表名
	// Parameter: uint32_t uMaxTable	分表的个数
	// Parameter: bool bSplit	是否分表，true分表，false 不分
	//************************************
	std::string getTableName( int32_t nTbId, std::string strTableName,uint32_t uMaxTable /*= 10*/,bool bSplit /*= true*/)
	{
		if (!bSplit ) return strTableName;
		int32_t nId = nTbId % uMaxTable;
		return StrFormatA("%s%" PRIi32,strTableName.c_str(),nId);
	}

	bool saveMoney2Bank(Json::Value& jvRet, int32_t nMid, int64_t nMoney, int32_t nGameId, int32_t nProductId /*= 1*/)
	{
		if (nMid < 0 || nMoney <= 0)
		{
			return false;
		}
		//	std::string strTableName = getTableName(nMid, strTb_GameInfo, 10);
	/*	Json::Value jvMoneyRet;
		bool bMoneySvrRet = CTcpMsg::setMoney(jvMoneyRet,nMid,-nMoney);
		if (bMoneySvrRet)
		{*/
			/*bMoneySvrRet =*/ CTcpMsg::setBank(jvRet,nMid,nMoney); //逻辑改了，只需要一个sebBank就可以了，不然会被刷钱
			if (jvRet["money"].isNull() || jvRet["freezemoney"].isNull()) return false;

			setBankLog(nMid, nMoney, Json::SafeConverToInt64(jvRet["money"]), Json::SafeConverToInt64(jvRet["freezemoney"]), nGameId, 1, nProductId);
			setWinLog(nMid, 7, 1, nMoney, "存保险箱");
			return true;
	//	}
	//	return false;
	}

	bool getMoneyFromBank(Json::Value& jvRet, int32_t nMid, int64_t nMoney, int32_t nGameId, int32_t nProductId /*= 1*/)
	{
		if (nMid < 0 || nMoney <= 0)
		{
			return false;
		}
		//	std::string strTableName = getTableName(nMid, strTb_GameInfo, 10);
	//	Json::Value jvMoneyRet;
		bool bMoneySvrRet = CTcpMsg::setBank(jvRet,nMid,-nMoney);
		if (bMoneySvrRet)
		{
	//		bMoneySvrRet = CTcpMsg::setMoney(jvMoneyRet,nMid,nMoney);	//money server 逻辑改了，setBank那里就直接把钱减了，防止作弊
			if (jvRet["money"].isNull() || jvRet["freezemoney"].isNull()) return false;

			setBankLog(nMid, nMoney, Json::SafeConverToInt64(jvRet["money"]), Json::SafeConverToInt64(jvRet["freezemoney"]), nGameId, 2, nProductId);
			setWinLog(nMid, 7, 0, nMoney, "取保险箱");
			return true;
		}
		return false;
	}

	int setBankLog( int32_t nMid, int64_t nAmount, int64_t nMoneyNow, int64_t nFreezeMoney, int32_t nGameId, uint8_t nType, int32_t nProductId /*= 1*/, int32_t nTomid /*= 0*/) 
	{
		GetDbMaster(dbMaster,0);

		int32_t tmCurSec = Util::getGMTTime_S();
		std::string strTableName = getTableName(nProductId, strTb_LogBank, 10);
		std::string strSql = StrFormatA("INSERT INTO %s SET mid='%" PRIi32 "',type='%" PRIi32 "',productid='%" PRIi32 "',amount='%" PRIi64 "',money='%" PRIi64 
			"',freezemoney='%" PRIi64 "',tomid='%" PRIi32 "',ctime='%" PRIi32 "'",
			strTableName.c_str(), nMid, nType, nProductId, nAmount, nMoneyNow, nFreezeMoney, nTomid, tmCurSec);
		LOGGER(E_LOG_INFO) << strSql.c_str();
		return dbMaster->ExeSQL((char*) strSql.c_str());
	}

	void setWinLog( int32_t nMid, int32_t nMode, int32_t nWflag, int64_t nMoney, std::string strDesc /*= ""*/) 
	{
		MySqlDBAccessMgr* dbMgr = ThreadResource::getInstance().getDBConnMgr();
		if (!dbMgr)
		{
			LOGGER(E_LOG_ERROR) << "getDBConnMgr is null, it did not be initted when server starting!";
			return ;
		}
		MySqlDBAccess* dbMaster = dbMgr->DBMaster();
		if (NULL == dbMaster) 
		{
			LOGGER(E_LOG_ERROR) << "MySqlDBAccess is null, it did not be initted when server starting!";
			return ;
		}

		Json::Value jvInfo;
		getOneById(jvInfo, nMid);
		int32_t nProductId = Json::SafeConverToInt32(jvInfo["productid"]);
		std::string strTableName = StrFormatA("%s%d", strTb_WinLog.c_str() ,nProductId%10==0?10:nProductId%10);
		int32_t tmCurSec = Util::getGMTTime_S();
		if(strDesc.empty()) strDesc = "金币充值变化";
		int64_t nMoneyTmp = abs(nMoney);
		nMoney = (nWflag == 0 ? nMoneyTmp : -1 * nMoneyTmp);
		int32_t nSId = Json::SafeConverToInt32(jvInfo["sid"]);
		int32_t nCId = Json::SafeConverToInt32(jvInfo["cid"]);
		int32_t nType = Json::SafeConverToInt32(jvInfo["ctype"]);
		int32_t nGameId = Json::SafeConverToInt32(jvInfo["gameid"]);
		int64_t nAfterMoney = Json::SafeConverToInt64(jvInfo["money"]);
		int64_t mBeforeMoney = nAfterMoney - nMoney;

		std::string strSql = StrFormatA("INSERT INTO %s SET after_money=%" PRIi64 ",before_money=%" PRIi64 ",mid='%" PRIi32 "',gameid='%" PRIi32 "',sid='%" PRIi32 "',cid='%" PRIi32 
			"',pid=%" PRIi32 ",ctype='%" PRIi32 "',wmode=%" PRIi32 ",wflag=%" PRIi32 ",money=%" PRIi64 ",wtime=%" PRIi32 ",wdesc='%s'",
			strTableName.c_str(), nAfterMoney, mBeforeMoney, nMid, nGameId, nSId, nCId, nProductId, nType, nMode, nWflag, nMoney, tmCurSec, strDesc.c_str());
		LOGGER(E_LOG_INFO) << strSql.c_str();
		dbMaster->ExeSQLNoRet((char*) strSql.c_str());
	}

	int addRegisterUserName(const std::string &username, const std::string &password)
	{
		int sitemid = CreateSiteMid();
		if (sitemid == 0) return 0;

		GetDbMaster(dbMaster,0);

		string query = StrFormatA("INSERT INTO %s SET sitemid=%d, username='%s',password='%s'", strTb_Username_Register.c_str(), sitemid, username.c_str(), password.c_str());
		dbMaster->ExeSQLNoRet((char*)query.c_str());
		return sitemid;
	}

	bool paySystemRechargeOther(const Json::Value &param)
	{
		GetDbMaster(dbMaster,false);

		Json::Value payinfo;
		std::string dealno = param["pdealno"].asString();
		if (dealno.empty())
		{
			LOGGER(E_LOG_INFO) << "dealno is invalid!";
			return false;
		}
		std::string sql = StrFormatA("SELECT * FROM %s WHERE pdealno='%s' AND pstatus=0", strTb_SystemRechargeOther.c_str(), dealno.c_str());
		auto result = dbMaster->Query(sql.c_str());
		if (result == NULL || !result->hasNext())
		{
			LOGGER(E_LOG_INFO) << "paySystemRechargeOther failed";
			if(result) delete result;
			return false;
		}
		int mid = result->getIntValue("mid");
		int exc = result->getIntValue("pexchangenum");
		std::string remark = result->getCharValue("remark");
		delete result;
		Json::Value userinfo;
		if (!GetUserInfo(mid, userinfo))
		{
			LOGGER(E_LOG_INFO) << "get user info failed";
			return false;
		}
		int wmode = 1;
		int sid = Json::SafeConverToInt32(userinfo["sid"]);
		int cid = Json::SafeConverToInt32(userinfo["cid"]);
		int pid = Json::SafeConverToInt32(userinfo["pid"]);
		int ctype = Json::SafeConverToInt32(userinfo["ctype"]);
		int ptype = Json::SafeConverToInt32(userinfo["ptype"]) == 1 ? 0 : 1;
		int gameid = Json::SafeConverToInt32(userinfo["gameid"]);
		if (!RedisLogic::addNewWin(dealno, 0, mid, MONEY_RECEIVE_BENEFITS, sid, pid, ctype, ptype, exc, "救济金(C++)", 30))
		{
			LOGGER(E_LOG_INFO) << "get add new win failed";
			return false;
		}
		RedisAccessMgr* redisMgr = ThreadResource::getInstance().getRedisConnMgr();
		std::string key = StrFormatA("MONNEW:%s", dealno.c_str());
		std::string value;
		if (!redisMgr->money()->HGET(key.c_str(), "status", value))
		{
			LOGGER(E_LOG_INFO) << "get from redis failed";
			return false;
		}
		redisMgr->money()->DEL(key.c_str());
		payinfo["mid"] = mid;
		payinfo["pexchangenum"] = exc;
		payinfo["ptype"] = ptype;
		std::string pmsg = ptype == 1 ? "赠送" : "扣除";
		float mon = (float)exc / 100;
		std::string content = StrFormatA("金币调整,%s金币%.2f", pmsg.c_str(), mon);
		if (!remark.empty())
		{
			content = StrFormatA("%s,原因:%s", content.c_str(), remark.c_str());
		}
		//TODO: ip is empty now
		feedBack(gameid, cid, sid, pid, ctype, content.c_str(), mid, userinfo["mnick"].asString().c_str(), "", "", "", 3, Json::SafeConverToInt32(userinfo["productid"]));
		Json::Value gameinfo;
		if (!GetGameInfo(mid, gameinfo))
		{
			LOGGER(E_LOG_INFO) << "get game info failed";
			return false;
		}
		bool bSennOk = CTcpMsg::sendMsg(mid, ptype, "救济金领取");
		LOGGER(E_LOG_INFO) << "Send Msg To Moeny Sever[mid:"<< mid << ";ptype:" << ptype << ";救济金领取;result:" << bSennOk << "]";

		int nowmoney = Json::SafeConverToInt32(gameinfo["money"]);
		// update to mysql
		std::string str = StrFormatA("UPDATE %s SET pstatus=1,pmoneyafter='%d' WHERE pdealno='%s' AND pstatus=0  LIMIT 1", strTb_SystemRechargeOther.c_str(), nowmoney, dealno.c_str());
		LOGGER(E_LOG_INFO) << str;
		return dbMaster->ExeSQL(str.c_str()) > 0;
	}

	int feedBack(int gameid, int cid, int sid, int pid, int ctype, const char* content, int mid,
		const char* nick, const char* phoneno, const char* pic, const char* ip, int status, int productid)
	{
		GetDbMaster(dbMaster,0);

		string strContent = dbMaster->EscapeString(content, strlen(content));
		string strPic     = dbMaster->EscapeString(pic, strlen(pic));
		string strNick    = dbMaster->EscapeString(nick, strlen(nick));
		string strPhoneno = dbMaster->EscapeString(phoneno, strlen(phoneno));

		int ipaddr  = ip ? Helper::ip2long(ip) : 0;

		dbMaster->ExeSQLNoRet("set names utf8mb4");
		string sql = StrFormatA("INSERT INTO ucenter.uc_feedback SET gameid=%d,productid=%d,cid=%d,pid=%d,sid=sid,ctype=%d,mnick='%s',content='%s',mid=%d,phoneno='%s',status=%d, pic='%s',ip=%d,ctime=%d",
			gameid, productid, cid, pid, ctype, strNick.c_str(), strContent.c_str(), mid, strPhoneno.c_str(), status, strPic.c_str(), ipaddr, time(NULL));
		int affectedRows = dbMaster->ExeSQL(sql.c_str());
		std::string strKey = StrFormatA("FBK|%d|%d", productid,mid);
		redisReply * reply = ThreadResource::getInstance().getRedisConnMgr()->common()->CommandV("del %s",strKey.c_str());
		freeReplyObject(reply);

		return affectedRows;
	}

	int resetPassword(int32_t nSiteMid, int32_t nSId, std::string strOldPsw, std::string strNewPsw) 
	{
		if (nSiteMid < 0 || nSId < 0 || strOldPsw.empty() || strNewPsw.empty() || strOldPsw == strNewPsw) return 0;

		GetDbMaster(dbMaster,0);

		std::string strTableName = nSId == 101 ? strTb_Phonenumber_Register : strTb_Username_Register;
		MD5 md5OldPsw(strOldPsw);
		strOldPsw = md5OldPsw.md5();
		std::string strSql = StrFormatA("SELECT password FROM %s WHERE sitemid=%" PRIi32 " LIMIT 1;",strTableName.c_str(),nSiteMid);
		MySqlResultSet* result = dbMaster->Query((char*)strSql.c_str());
		if ( !result ) return 0;
		if(!result->hasNext()) 
		{
			delete result;
			return 0;
		}

		std::string strSvrPsw = result->getCharValue(0);
		delete result;
		if (strOldPsw != strSvrPsw) return -1;

		MD5 md5NewPsw(strNewPsw);
		strNewPsw = md5NewPsw.md5();
		strSql = StrFormatA("UPDATE %s SET password='%s' WHERE sitemid=%" PRIi32 " LIMIT 1;",strTableName.c_str(), strNewPsw.c_str(), nSiteMid);
		return dbMaster->ExeSQL((char*)strSql.c_str()) ? 1 : 0;
	}

	int getSitemidByName(const std::string &username, const std::string& password)
	{
		GetDbMaster(dbMaster,0);

		std::string name = dbMaster->EscapeString(username.c_str(), username.length());
		MD5 md5 = MD5(password);
		std::string md5pwd = md5.md5();
		std::string query = StrFormatA("SELECT sitemid,username FROM %s where username='%s' AND (password='%s' or password='%s') LIMIT 1", 
			strTb_Username_Register.c_str(), name.c_str(), password.c_str(), md5pwd.c_str());
		auto result = dbMaster->Query((char*)query.c_str());
		if (result == NULL)
		{
			return 0;
		}
		if(result->hasNext())
		{
			int sitemid = result->getIntValue("sitemid");
			delete result;
			return sitemid;
		}
		delete result;
		return 0;
	}


	bool getUserNameBySitemid(int32_t nSitemId, uint32_t nSid , std::string& strUserName)
	{
		GetDbMaster(dbMaster,false);

		std::string strName = strTb_Username_Register;
		std::string strFiled = "username";
		if(nSid == 101)
		{
			strName = strTb_Phonenumber_Register;
			strFiled = "phoneno";
		}
		std::string strSql = StrFormatA("SELECT %s FROM %s WHERE sitemid='%" PRIi32 "' LIMIT 1",strFiled.c_str(),strName.c_str(),nSitemId);
		MySqlResultSet* result = dbMaster->Query((char*)strSql.c_str());
		if (NULL == result) return false;
		if(!result->hasNext())
		{
			delete result;
			return false;
		}
		strUserName = result->getCharValue(0);
		delete result;
		return true;
	}

	bool guestBindAccount(std::string& username, std::string& password, int mid) 
	{
		GetDbMaster(dbMaster,false);
        string query;
        int sitemid = getSitemidByMid(mid);
        if (sitemid > 0)
        {
            query = StrFormatA("update ucenter.uc_register_username SET username='%s', password='%s' where sitemid=%d", username.c_str(), password.c_str(), sitemid);
        }
        else
        {
            sitemid = CreateSiteMid();
            password = MD5(password).md5();

            if (!sitemid)
            {
                return false;
            }

            query = StrFormatA("INSERT into ucenter.uc_register_username SET username='%s', password='%s', sitemid=%d", username.c_str(), password.c_str(), sitemid);
        }

		int flag = dbMaster->ExeSQL(query.c_str());

		string tbgameinfo = getTableName(mid, strTb_GameInfo, 10);
		string tbuserinfo = getTableName(mid, strTb_UserInfo, 10);

		string sql = StrFormatA("UPDATE %s SET sitemid='%d',sid=102 WHERE mid='%d' LIMIT 1", tbuserinfo.c_str(), sitemid, mid);
		int cnt = dbMaster->ExeSQL(sql.c_str());

		sql = StrFormatA("UPDATE %s SET sitemid='%d',sid=102 WHERE mid=%d LIMIT 1", strTb_SitemId2Mid.c_str(), sitemid, mid);
		cnt = dbMaster->ExeSQL(sql.c_str());

		Json::Value oldUser;
		getOneById(oldUser, mid);
		int oldSitemid = 0;
		if (oldUser.size() > 0)
			oldSitemid = Json::SafeConverToInt32(oldUser["sitemid"]);

		sql = StrFormatA("UPDATE ucenter.uc_account_binding SET sitemid='%d' WHERE sitemid='%d' LIMIT 1", sitemid, oldSitemid);
		cnt = dbMaster->ExeSQL(sql.c_str());

		sql = StrFormatA("delete from ucenter.uc_register_ios where sitemid='%d'", oldSitemid);
		cnt = dbMaster->ExeSQL(sql.c_str());

		sql = StrFormatA("delete from ucenter.uc_register_android where sitemid='%d'", oldSitemid);
		cnt = dbMaster->ExeSQL(sql.c_str());

		//Loader_Redis::account()->hSet(Config_Keys::bangdingRecord(), oldSitemid, sitemid.':102');//记录游客绑定新账号的sitemid影射
		RedisLogic::GetRdMinfo(oldSitemid)->DEL(StrFormatA("UMID%d|%d", oldSitemid, 100).c_str());
		map<string,string> kv;
		kv["sitemid"] = StrFormatA("%d", sitemid);
		kv["sid"] = StrFormatA("%d", 102);
		RedisLogic::GetRdMinfo(mid)->HMSET(StrFormatA("UCNF%d", mid).c_str(), kv);

		return flag > 0;
	}

	bool guestBinding(int sitemid, int mid) 
	{
		if (sitemid < 1 || mid < 1)  return false;

		GetDbMaster(dbMaster,false);

		string tbgameinfo = getTableName(mid, strTb_GameInfo, 10);
		string tbuserinfo = getTableName(mid, strTb_UserInfo, 10);

		string sql = StrFormatA("UPDATE %s SET sitemid='%d',sid=101 WHERE mid='%d' LIMIT 1", tbuserinfo.c_str(), sitemid, mid);
		dbMaster->ExeSQL(sql.c_str());

		sql = StrFormatA("UPDATE %s SET sitemid='%d',sid=102 WHERE mid=%d LIMIT 1", strTb_SitemId2Mid.c_str(), sitemid, mid);
		int cnt = dbMaster->ExeSQL(sql.c_str());

		Json::Value oldUser;
		getOneById(oldUser, mid);
		int oldSitemid = 0;
		if (oldUser.size() > 0)
			oldSitemid = Json::SafeConverToInt32(oldUser["sitemid"]);

		ThreadResource::getInstance().getRedisConnMgr()->account()->HSET("BRD", StrFormatA("%d",oldSitemid).c_str(), StrFormatA("%d:101",sitemid).c_str()); //记录游客绑定新账号的sitemid影射
		ThreadResource::getInstance().getRedisConnMgr()->minfo(oldSitemid)->DEL(StrFormatA("UMID%d|%d", oldSitemid, 100).c_str());
		map<string,string> kv;
		kv["sitemid"] = StrFormatA("%d", sitemid);
		kv["sid"] = StrFormatA("%d", 101);
		ThreadResource::getInstance().getRedisConnMgr()->minfo(mid)->HMSET(StrFormatA("UCNF%d", mid).c_str(), kv);
		return cnt > 0;
	}

	/**
	 * 手机号码注册
	 * @param {string} $phoneNum 手机号码
	 * @param  type 1 手机号码注册  2 android游客账号绑定
	 * @return int
	 */
	bool registerByPhoneNumber(Json::Value &data, int type) 
	{
		GetDbMaster(dbMaster,false);

		string phoneno = dbMaster->EscapeString(data["phoneno"].asString());
		string password = data["password"].asString();
		int mid, sitemid;

		if (phoneno.empty() || password.empty()) 
		{
			return false;
		}

        password = MD5(password).md5();

		//if (type == 2) 
		//{
		mid = Json::SafeConverToInt32(data["mid"], 0);
		//}
        if (mid > 0)
        {
            sitemid = getSitemidByMid(mid);
        }
        else
        {
            sitemid = CreateSiteMid();
        }

        data["sitemid"] = sitemid;

		if (sitemid) 
		{
			string query = StrFormatA("INSERT INTO ucenter.uc_register_phonenumber SET sitemid='%d',phoneno='%s',password='%s'", sitemid, phoneno.c_str(), password.c_str());
			dbMaster->ExeSQL(query.c_str());
			bool flag = type == 1 ? !insert(data).empty() : (bool) guestBinding(sitemid, mid); //用户信息表

			return flag;
		}

		return false;
	}

	/**
 * 用户名注册
 */
	int32_t registerByUserName(Json::Value &data) 
	{
		Json::Value jsRet;
		return registerByUserName(data,jsRet);
	}

	int32_t registerByUserName( Json::Value &data,Json::Value &jsRet )
	{
		GetDbMaster(dbMaster,-1);

		string username = dbMaster->EscapeString(data["username"].asString());
		string password = data["password"].asString();

		if (username.empty() || password.empty()) return -1;

		password = MD5(password).hexdigest();
		int sitemid;
		if (!data.isMember("sitemid") || data["sitemid"].type() != Json::intValue) 
		{
			data["sitemid"] = sitemid = CreateSiteMid();
		} 
		else 
		{
			sitemid = Json::SafeConverToInt32(data["sitemid"]);
		}

		if (sitemid != -1) 
		{
			string query = StrFormatA("INSERT INTO ucenter.uc_register_username SET sitemid=%d,username='%s',password='%s'",
				sitemid, username.c_str(), password.c_str());
			int ret = dbMaster->ExeSQL((char*)query.c_str());
			jsRet = insert(data); //用户信息表
			return Json::SafeConverToInt32(jsRet["mid"],-1);
		}

		return -1;
	}

	Json::Value insert(Json::Value userInfo) 
	{
		Json::Value ret;
		if (!userInfo.size())  return ret;

		GetDbMaster(dbMaster,ret);

		int now = time(NULL);
		int sitemid = Json::SafeConverToInt32(userInfo["sitemid"]);
		string mnick = dbMaster->EscapeString(userInfo["mnick"].asString());
		int sid = Json::SafeConverToInt32(userInfo["sid"]);
		int cid = Json::SafeConverToInt32(userInfo["cid"]);
		int pid = Json::SafeConverToInt32(userInfo["pid"]);
		int ctype = Json::SafeConverToInt32(userInfo["ctype"]);
		string mtime = StrFormatA("{\"%d\":%d}", Json::SafeConverToInt32(userInfo["productid"]), now);
		int rand_sex = GetRandInt(1,2);
		int sex = Json::SafeConverToInt32(userInfo["sex"],rand_sex);
		string hometown = userInfo.isMember("hometown") ? dbMaster->EscapeString(userInfo["hometown"].asString()) : "";
		string device_name = dbMaster->EscapeString(userInfo["device_name"].asString());
		string os_version = dbMaster->EscapeString(userInfo["os_version"].asString());
		int net_type = Json::SafeConverToInt32(userInfo["net_type"]);
		string strIp = userInfo["ip"].asString();
		long lIp = Helper::ip2long(strIp.c_str());
		string device_no = userInfo["device_no"].asString();
		int gameid = Json::SafeConverToInt32(userInfo["gameid"]);
		int productid = Json::SafeConverToInt32(userInfo["productid"]);
		int mid = _createMid(sitemid, sid, strIp, device_no);
		string openid = Json::SafeConverToStr(userInfo["openudid"]);
		int nVersion = Json::SafeConverToInt32(userInfo["version"]);

		if (specialAccount.find(mid) != specialAccount.end()) //如果新注册的用户与保留的特殊账号相同，则重新生成一个mid
		{
			mid = _createMid(sitemid, sid, strIp, device_no);
		}

		string table = getTableName(mid,strTb_UserInfo,10);

		std::string query = StrFormatA("INSERT INTO %s SET mid='%d', sitemid='%d', mnick='%s', sid='%d', mtime='%s',"
			"ip=%lld, sex=%d,hometown='%s', cid=%d,pid=%d, ctype=%d, devicename='%s',"
			"osversion='%s', nettype=%d, openid='%s', ctime=%d, productid=%d,  versions=%d,"
			"mactivetime=0, mentercount=0",
			table.c_str(), mid, sitemid, mnick.c_str(), sid, mtime.c_str(),lIp, sex,
			hometown.c_str(), cid, pid, ctype, device_name.c_str(), os_version.c_str(), net_type,
			openid.c_str(), time(NULL), productid, nVersion);

		ULOGGER(E_LOG_INFO, mid) << query;

		int rows = dbMaster->ExeSQL(query.c_str());

		if (rows > 0) 
		{
			//更新redis sitemid和mid的对应关系
			std::string cachekey = getSitemid2MidKey(sitemid , sid);
			RedisLogic::setMidWithCacheKey(cachekey, mid, false, false);

			int registerMoney = 300;
			//int registerMoney = 1000000;	//for test
			table = getTableName(mid, strTb_GameInfo);
			query = StrFormatA("INSERT INTO %s SET mid=%d, gameid=%d, money=%d, coins=0, freezemoney=0, roll=0, roll1=0, exp=0, sid=0", table.c_str(), mid, gameid, registerMoney);
			int flag = dbMaster->ExeSQL(query.c_str());

			//registerMoney = cid == 15 ? Config_Money::spfirstin : Config_Money::firstin[userInfo['gameid']];
			//registerMoney = Config_Money::firstin[gameid];
			//Logs::factory()->addWin(gameid,mid, 1,sid, cid, pid,ctype, 0,registerMoney,"注册送金币");//首注加钱
			// 初始化缓存
			std::string strMonKey = StrFormatA("MON:%d",mid);
			std::map<std::string,std::string> mapMonKV;
			mapMonKV["svid"] = "0";
			mapMonKV["money"] = StrFormatA("%d",registerMoney); //"300";
			mapMonKV["smoney"] = "0";
			mapMonKV["roll"] = "0";
			mapMonKV["roll1"] = "0";
			mapMonKV["coin"] = "0";
			mapMonKV["exp"] = "0";
			mapMonKV["lasttime"] = "0";
			RedisLogic::GetRdMoney()->HMSET(strMonKey.c_str(), mapMonKV);

			query = StrFormatA("INSERT INTO %s SET mid=%d,productid=%d,ip=%lld,deviceno='%s',ctime=%d",
				strTb_StatRegister.c_str(),mid, productid, lIp, device_no.c_str(), time(NULL));
			dbMaster->ExeSQLNoRet((char*)query.c_str());
            
            // 初始化搜索参数
			std::string strCountry = TrimStr(userInfo["country"].asString());//国家/64个字符
			std::string strProvince = userInfo["province"].asString();//省份/64个字符
			std::string strCity = userInfo["city"].asString();		//城市/64个字符
			if (strCountry.empty())	//客户端没传，通过IP地址计算
			{
				strProvince = "";
				strCity = "";
				std::vector<string> vsIpInfo = Lib_Ip::find(strIp.c_str());
				if (vsIpInfo.size() > 0) strCountry = vsIpInfo[0];
				if (vsIpInfo.size() > 1) strProvince = vsIpInfo[1];
				if (vsIpInfo.size() > 2) strCity = vsIpInfo[2];
			}
			std::string strEquipmentModel = userInfo["equipment_model"].asString();	//手机设备型号/128个字符
			std::string strNetWorking = userInfo["networking"].asString();//联网方式/10个字符  3G 4G等等
			strCountry = dbMaster->EscapeString(strCountry);
			strProvince = dbMaster->EscapeString(strProvince);
			strCity = dbMaster->EscapeString(strCity);
			strEquipmentModel = dbMaster->EscapeString(strEquipmentModel);
			strNetWorking = dbMaster->EscapeString(strNetWorking);
			int nSex = Json::SafeConverToInt32(userInfo["sex"]);
			int nVersions = Json::SafeConverToInt32(userInfo["versions"]);

			std::string username = userInfo["username"].asString();
            query = StrFormatA("INSERT INTO ucenter.uc_stat_pay SET mid=%d, productid='%d',money='%d',freezemoney=0,charge_amount=0"
                               ", withdraw_amount=0,tax=0,username='%s',mnick='%s',alipay='',ip='%lld',deviceno='%s',ctime='%d',mstatus=10"
							   ", country='%s', province='%s', city='%s', equipment_model='%s', networking='%s', versions='%d', sex='%d'"
                               ,mid, productid, registerMoney, username.c_str(), mnick.c_str(), lIp, device_no.c_str(), time(NULL)
							   ,strCountry.c_str(),strProvince.c_str(),strCity.c_str(),strEquipmentModel.c_str(),strNetWorking.c_str(),nVersions,nSex);
            dbMaster->ExeSQLNoRet(query.c_str());

			// 初始化保险箱密码
			if (productid == 22) 
			{
				map<string, string> aInfo;
				aInfo["bankPWD"]= MD5("888888").hexdigest();
				setUserInfo(mid, aInfo);
			}
			setWinLog(mid, 5, 0, 300, "注册送金币");
			if (flag > 0) getOneById(ret, mid);
		}

		return ret;
	}

	int _createMid(int sitemid, int sid, string ip, string device_no) 
	{
		GetDbMaster(dbMaster,-1);

		int now = time(NULL);
		string query = StrFormatA("INSERT INTO %s SET sitemid='%d',sid=%d,deviceno='%s',ip='%s',time=%d",
			strTb_SitemId2Mid.c_str(), sitemid, sid, device_no.c_str(), ip.c_str(), now);
		int ret = dbMaster->ExeSQL(query.c_str());

		if (ret != -1)
			return dbMaster->GetLastInsertId();

		return -1;
	}

	bool userNameBinding(int sitemid, string phoneno, int productid) 
	{
		GetDbMaster(dbMaster,false);

		phoneno = dbMaster->EscapeString(phoneno);

		if (!sitemid || phoneno.empty()) {
			return false;
		}

		string sql = StrFormatA("SELECT sitemid FROM ucenter.uc_register_phonenumber WHERE phoneno = '%s' LIMIT 1",
			phoneno.c_str());
		MySqlResultSet * res = dbMaster->Query(sql.c_str());
		if (res && res->getRowNum() > 0) //判断之前是否已经用该手机号码注册
		{
			delete res;
			return false;
		}

		delete res;
		sql = StrFormatA("SELECT sitemid FROM ucenter.uc_account_binding WHERE ( sitemid=%d OR (phoneno='%s' and productid='%d')) and status=1 LIMIT 1",
			sitemid, phoneno.c_str(), productid);
		res = dbMaster->Query(sql.c_str());
		if (res && res->getRowNum() > 0) 
		{
			delete res;
			return false;
		}

		if(res) delete res;
		sql = StrFormatA("INSERT INTO ucenter.uc_account_binding SET sitemid='%d',phoneno='%s',ctime='%d',productid='%d'"
			"ON DUPLICATE KEY UPDATE sitemid ='%d',phoneno = '%s',ctime ='%d',productid='%d'", sitemid, phoneno.c_str(), time(NULL),
			productid, sitemid, phoneno.c_str(), time(NULL), productid);

		int cnt = dbMaster->ExeSQL(sql.c_str());

		return true;
	}

	bool setMessageLogs(int mid, string phoneno, string content, int type, int result)
	{
		GetDbMaster(dbMaster,false);

		phoneno = dbMaster->EscapeString(phoneno);
		content = dbMaster->EscapeString(content);

		string sql = StrFormatA("INSERT INTO ucenter.uc_logmessage SET mid=%d,phoneno='%s',content='%s',ctime='%s',status='%d',type='%d',msgid='%d'",
			mid, phoneno.c_str(), content.c_str(), Util::formatGMTTimeS().c_str(), result, type, 0);
		int flag = dbMaster->ExeSQL(sql.c_str());
		return true;
	}

	bool getDeviceNoByMid(int mid, std::string &deviceNo)
	{
		GetDbMaster(dbMaster,false);

		std::string query = StrFormatA("SELECT deviceno FROM %s WHERE mid='%d'", strTb_SitemId2Mid.c_str(), mid);
		auto result = dbMaster->Query(query.c_str());
		if (result && result->hasNext())
		{
			deviceNo = result->getCharValue("deviceno");
			delete result;
			return true;
		}
		if(result) delete result;
		return false;
	}

	bool getMidArrayByDeviceNo(const std::string &deviceNo, std::vector<int> &vec)
	{
		GetDbMaster(dbMaster,false);

		std::string devno = dbMaster->EscapeString(deviceNo);
		std::string query = StrFormatA("SELECT mid FROM %s WHERE deviceno='%s'", strTb_SitemId2Mid.c_str(), devno.c_str());
		auto result = dbMaster->Query(query.c_str());
		if (result == NULL)
		{
			LOGGER(E_LOG_ERROR) << "query failed";
			return false;
		}
		while (result->hasNext())
		{
			vec.push_back(result->getIntValue("mid"));
		}
		delete result;
		return true;
	}

	//登录信息保存数据库记录
	bool setLoginRecord(const Json::Value& jvUser) 
	{
		GetDbMaster(dbMaster,false);
		std::string strIpArr = Helper::implode("",jvUser["ip_arr"]);
	
		int nMid = Json::SafeConverToInt32(jvUser["productid"]);
		std::string strTableName = SqlLogic::getTableName(nMid,"logchip.log_login");
		std::string strSql = StrFormatA("INSERT INTO %s SET ip='%s',ip_arr='%s',ctime='%s',mid='%" PRIi32 "',productid='%" PRIi32 "',deviceno='%s'",
			strTableName.c_str(),jvUser["ip"].asString().c_str(),strIpArr.c_str(),Util::formatGMTTimeS().c_str(),Json::SafeConverToInt32(jvUser["mid"]),
			nMid,jvUser["device_no"].asString().c_str());
		
		return dbMaster->ExeSQL(strSql.c_str()) > 0 ? true : false;
	}

	/**
     * 通过手机号获取账号和密码
     */
    bool getPasswordByphoneNo(Json::Value& jvRet, std::string strPhoneNo, int32_t nProductId /*= 1*/) 
	{
		if (strPhoneNo.empty()) return false;

		GetDbMaster(dbMaster,false);
		strPhoneNo = dbMaster->EscapeString(strPhoneNo);
		if (strPhoneNo.empty()) return false;
		std::string strTableName = strTb_Phonenumber_Register;
		std::string strSql = StrFormatA("SELECT sitemid FROM %s WHERE phoneno='%s' LIMIT 1",strTableName.c_str(),strPhoneNo.c_str());

		MySqlResultSet* result = dbMaster->Query(strSql.c_str());
		if(NULL == result || !result->hasNext())
		{
			if(result) delete result;

			strTableName = strTb_Username_Register;
			strSql = StrFormatA("SELECT sitemid FROM %s WHERE phoneno='%s' and productid='%d' LIMIT 1",strTb_Account_Binding.c_str(), strPhoneNo.c_str(), nProductId);
			result = dbMaster->Query(strSql.c_str());
			if(NULL == result || !result->hasNext())
			{
				if(result) delete result;
				return false;
			}
		}
		int32_t nSiteMid = result->getIntValue(0);
		delete result;
		std::string strPswSrc = StrFormatA("%d", GetRandInt(100000, 999999));
		MD5 md5(strPswSrc);
		std::string strPswMd5 = md5.md5();
		strSql = StrFormatA("UPDATE %s SET password='%s' WHERE sitemid='%d' LIMIT 1",strTableName.c_str(), strPswMd5.c_str(), nSiteMid);
		dbMaster->ExeSQLNoRet(strSql.c_str());
		if (strTableName == strTb_Phonenumber_Register)
		{
			jvRet["username"] = strPhoneNo;
			jvRet["password"] = strPswSrc;
		}
		else
		{
			strSql = StrFormatA("SELECT username FROM %s WHERE sitemid='%d'",strTableName.c_str(), nSiteMid);
			result = dbMaster->Query(strSql.c_str());
			if(NULL == result || !result->hasNext())
			{
				if(result) delete result;
				return false;
			}
			jvRet["username"] = result->getCharValue(0);
			jvRet["password"] = strPswSrc;
			delete result;
		}

		return true;
    }

	bool bindingAlipay(const Json::Value& jvAliInfo) 
	{
		GetDbMaster(dbMaster,false);

		int32_t nMid = Json::SafeConverToInt32(jvAliInfo["mid"]);
		std::string strAliPay = Json::SafeConverToStr(jvAliInfo["alipay"]);
		strAliPay = TrimStr(strAliPay);
		if (nMid <= 0 || strAliPay.empty())
		{
			return false;
		}
		std::string strSql = StrFormatA("delete from %s where mid= %d",strTB_Aliuser.c_str(),nMid);
		dbMaster->ExeSQLNoRet(strSql.c_str());
		std::string strAliName = Json::SafeConverToStr(jvAliInfo["aliname"]);
		strAliName = dbMaster->EscapeString(strAliName);
		strSql = StrFormatA("INSERT INTO %s SET mid=%" PRIi32 ", alipay='%s',aliname='%s',gameid=%" PRIi32,
			strTB_Aliuser.c_str(), nMid, strAliPay.c_str(), strAliName.c_str(), Json::SafeConverToInt32(jvAliInfo["gameid"]));
		int nCout = dbMaster->ExeSQL(strSql.c_str());
		if (nCout > 0)
		{
			//更新缓存，不然下次登录又会变成没绑定吧,PHP登录的时候，直接读了数据库，都不走redis了，他那个GetUserInfo 最后一个参数是False
			Json::Value info;
			info["alipay"] = strAliPay;
			info["aliname"] = strAliName;
			return RedisLogic::SetUserInfo(nMid, info, 2 * 24 * 3600); //写入redis
			return true;
		}
		return false ;
	}

	int addLogin(Json::Value& user)
	{
		GetDbMaster(dbMaster, -1);
		int32_t mid = Json::SafeConverToInt32(user["mid"]);
		int32_t productid = Json::SafeConverToInt32(user["productid"]);
		string sproductid = user["productid"].asString();
		int32_t gameid = Json::SafeConverToInt32(user["gameid"]);

		Json::Value aMactivetime = user["aMactivetime"];
		aMactivetime[sproductid] = (int64_t)time(NULL);

		Json::Value aMentercount = user["aMentercount"];
		aMentercount[sproductid] =Json::SafeConverToInt32(aMentercount[sproductid]) + 1;

		Json::FastWriter jWriter;
		std::string mactivetime = Helper::trim(jWriter.write(aMactivetime).c_str());
		std::string mentercount = Helper::trim(jWriter.write(aMentercount).c_str());

		if (user["sid"] != 100)
		{
			return -1;
		}

		std::string mtimeadd = "";
		if (Json::SafeConverToInt32(user["mentercount"]) == 0)
		{
			Json::Value aMtime = user["aMtime"];
			aMtime[gameid] = (int64_t)time(NULL);
			std::string mtime = jWriter.write(aMtime);
			mtimeadd = ",mtime = " + mtime;
		}
		std::string strVersions = user["versions"].asString();
		strVersions = dbMaster->EscapeString(strVersions);
		std::string strOsVersion = user["osversion"].asString();
		strOsVersion = dbMaster->EscapeString(strOsVersion);
		std::string strMNick= user["mnick"].asString();
		strMNick = dbMaster->EscapeString(strMNick);

		string strSql = StrFormatA("update ucenter.uc_userinfo%d set mactivetime='%s', mentercount='%s', "
								   "cid='%d', pid='%d', ctype='%d', devicename='%s', versions='%s' , osversion='%s' , nettype='%d', mnick='%s' where mid=%d" ,
								   mid % 10 , mactivetime.c_str() , mentercount.c_str(), 
								   Json::SafeConverToInt32(user["cid"]),Json::SafeConverToInt32(user["pid"]), Json::SafeConverToInt32(user["ctype"]),user["devicename"].asString().c_str(),
								   strVersions.c_str() , strOsVersion.c_str() , Json::SafeConverToInt32(user["nettype"]) , strMNick.c_str(), mid);

		int flag = SqlLogic::doMasterSql(strSql.c_str());
        
        // 更新搜索参数
        //$username = $this->getUserNameBySitemid($userInfo['sitemid'],102);
        //$query = "update ucenter.uc_stat_pay SET username='{$username}',mnick='{$userInfo['mnick']}',alipay='{$userInfo['alipay']}',ip='{$ip}' where mid=$mid";
        std::string username;
        SqlLogic::getUserNameBySitemid(Json::SafeConverToUInt32(user["sitemid"]), 102, username);
        std::string mnick = user["mnick"].asString();
		std::string alipay = user["alipay"].asString();
		std::string strIp = user["ip"].asString();
        long ip = Helper::ip2long(strIp.c_str());

		std::string strCountry = TrimStr(user["country"].asString());//国家/64个字符
		std::string strProvince = user["province"].asString();//省份/64个字符
		std::string strCity = user["city"].asString();		//城市/64个字符
		if (strCountry.empty())	//客户端没传，通过IP地址计算
		{
			strProvince = "";
			strCity = "";
			std::vector<string> vsIpInfo = Lib_Ip::find(strIp.c_str());
			if (vsIpInfo.size() > 0) strCountry = vsIpInfo[0];
			if (vsIpInfo.size() > 1) strProvince = vsIpInfo[1];
			if (vsIpInfo.size() > 2) strCity = vsIpInfo[2];
		}
		std::string strEquipmentModel = user["equipment_model"].asString();	//手机设备型号/128个字符
		std::string strNetWorking = user["networking"].asString();//联网方式/10个字符  3G 4G等等
		strCountry = dbMaster->EscapeString(strCountry);
		strProvince = dbMaster->EscapeString(strProvince);
		strCity = dbMaster->EscapeString(strCity);
		strEquipmentModel = dbMaster->EscapeString(strEquipmentModel);
		strNetWorking = dbMaster->EscapeString(strNetWorking);
		int nSex = Json::SafeConverToInt32(user["sex"]);
		int nVersions = Json::SafeConverToInt32(user["versions"]);


        strSql = StrFormatA("update ucenter.uc_stat_pay SET username='%s', mnick='%s', alipay='%s', ip='%ld'"
			", country='%s', province='%s', city='%s', equipment_model='%s', networking='%s', versions='%d', sex='%d'"
			" where mid=%d"
              ,username.c_str(), mnick.c_str(), alipay.c_str(), ip
			  ,strCountry.c_str(),strProvince.c_str(),strCity.c_str(),strEquipmentModel.c_str(),strNetWorking.c_str(),nVersions,nSex
			  , mid);
        SqlLogic::doMasterSql(strSql.c_str());
        
		if (flag > 0)
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
			std::string cache_key = StrFormatA("UCNF%d", mid);
			std::vector<std::string> vsFields;
			vsFields.push_back("mactivetime");
			vsFields.push_back("versions");
			vsFields.push_back("ip");
			vsFields.push_back("devicename");
			vsFields.push_back("osversion");
			vsFields.push_back("nettype");
			vsFields.push_back("cid");
			vsFields.push_back("pid");
			vsFields.push_back("sid");
			vsFields.push_back("ctype");
			vsFields.push_back("mnick");
			vsFields.push_back("iconid");

			std::map<std::string , std::string> mpRet;
			if (!redis->HMGET(cache_key.c_str() , vsFields , mpRet))
			{
				return -1;
			}

			user["iconid"] = atoi(mpRet["iconid"].c_str());

			mpRet["mactivetime"] = mactivetime;
			mpRet["mentercount"] = mentercount;
			mpRet["versions"] = user["versions"].asString();
			mpRet["pid"] = user["pid"].asString();
			mpRet["ip"] = user["ip"].asString();
			mpRet["devicename"] = user["devicename"].asString();
			mpRet["osversion"] = user["osversion"].asString();
			mpRet["nettype"] = user["nettype"].asString();
			mpRet["cid"] = user["cid"].asString();
			mpRet["sid"] = user["sid"].asString();
			mpRet["ctype"] = user["ctype"].asString();
			mpRet["mnick"] = user["mnick"].asString();

			redis->HMSET(cache_key.c_str() , mpRet);
		}

		std::string c_ip = user["ip"].asString();
		vector<string> ip_arr = Lib_Ip::find(c_ip.c_str());
		std::string ips = "";
		if (ip_arr.size() > 0)
		{
			vector<string>::iterator it = ip_arr.begin();
			for (; it != ip_arr.end(); ++it)
			{
				ips += *it;
			}
		}
		strSql = StrFormatA("insert into behavior.user_device(`device`,`ip`,`time`,`addr`) values ('%s', '%d', %ld, '%s') on duplicate key update ip='%d', time=%ld, addr='%s '" ,
			user["device_no"].asString().c_str(), Helper::ip2long(c_ip.c_str()), (int64_t)time(NULL), ips.c_str(), Helper::ip2long(c_ip.c_str()), (int64_t)time(NULL), ips.c_str() );
		SqlLogic::doMasterSql(strSql.c_str());

		return 0;
	}
}
