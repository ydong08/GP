#pragma once

#include <string.h>
#include <string>
#include "json/json.h"
//#include "GameInfo.h"

namespace SqlLogic
{
	bool GetUserInfo(int32_t nMid , Json::Value& info, bool bUseCache = true);
	int  GetSiteMidByKey(std::string deviceNo, int productId);

	int getSitemidByIosKey(std::string strDeviceNo, std::string strOpenudId, int nProductId = 1);
	bool GetDeviceForbidInfo(std::string& ip, std::string& device_no, std::string& openudid);

	bool GetDeviceForbidInfoByMid(int32_t nMid);

	int getSitemidByPhoneNumber(std::string& phoneNo , std::string& passwd);
	int getSitemidByuserName(std::string& userName , std::string& strPsw);
	int getAccountByBinding(std::string& phoneNo , std::string& passwd, int nProductId);
	uint32_t doMasterSql(const char* sql);

	int getOneBySitemid(int32_t sitemid , int32_t sid , Json::Value& user, bool inCache);
	int getSpecialMidBySitemid(int32_t sitemid);
	int sitemid2mid(int32_t sitemid , int32_t sid);
	std::string getSitemid2MidKey(int32_t sitemid , int32_t sid);

//	bool SetRegAliPayInfo(int uid, CBindAliPayInfo& info, bool updata = false);
//	bool GetRegAliPayInfo(int uid, CBindAliPayInfo& info);

	std::string getBankPWD(int32_t nMid);
	bool setUserInfo( int32_t uMid, std::map<std::string,std::string> mpInsert);
	int64_t limitCount( std::string strKey, std::string strTtype, int64_t nCount, bool bUpdate = true, int32_t nExpire = 86400);
	std::string getTableName( int32_t nTbId, std::string strTableName,uint32_t uMaxTable = 10,bool bSplit = true );  
	int  CreateSiteMid();

	bool saveMoney2Bank(Json::Value& jvRet, int32_t nMid, int64_t nMoney, int32_t nGameId, int32_t nProductId = 1);
	bool getMoneyFromBank(Json::Value& jvRet, int32_t nMid, int64_t nMoney, int32_t nGameId, int32_t nProductId = 1);
	int setBankLog( int32_t nMid, int64_t nAmount, int64_t nMoneyNow, int64_t nFreezeMoney, int32_t nGameId, uint8_t nType, int32_t nProductId = 1, int32_t nTomid = 0);
	void setWinLog( int32_t nMid, int32_t nMode, int32_t nWflag, int64_t nMoney, std::string strDesc="");

	int resetPassword(int32_t nSiteMid, int32_t nSId, std::string strOldPsw, std::string strNewPsw);
	int getSitemidByName(const std::string& username, const std::string& password);
	bool getUserNameBySitemid(int32_t nSitemId, uint32_t nSid , std::string& strUserName);


	//随机生成的账号写入uc_register_username
	int addRegisterUserName(const std::string& username, const std::string& password);

	bool paySystemRechargeOther(const Json::Value& param);
	bool guestBindAccount(std::string& username, std::string& password, int mid);
	/**
     * 游客绑定手机号码
     */
    bool guestBinding(int sitemid, int mid);
    /**
    * 游客绑定点乐账号
    */
    bool registerByPhoneNumber(Json::Value &data, int type = 1);
    /**
     * 用户名注册
     */
    int32_t registerByUserName(Json::Value &data);
	int32_t registerByUserName(Json::Value &data,Json::Value &jsRet);
	/**
     * 用户注册
     * @author gaifyyang
     * @param  array userInfo 用户信息
     * @return array
     */
    Json::Value insert(Json::Value userInfo);
	/**
     * 得到自增mid
     */
    int _createMid(int sitemid, int sid, std::string ip, std::string device_no);
	/**
     * 用户名绑定手机（开始绑定）
     */
    bool userNameBinding(int sitemid, std::string phoneno, int productid = 1);
	/*
     * 插入发送短信记录
     */
    bool setMessageLogs(int mid, std::string phoneno, std::string content, int type, int result);

	// 获取mid当前设备号
	bool getDeviceNoByMid(int mid, std::string& deviceNo);
	// 根据设备号获取该设备注册的账号mid
	bool getMidArrayByDeviceNo(const std::string& deviceNo, std::vector<int>& vec);

	bool getOneById(Json::Value& jvRet, int32_t nMid, bool bUseCache = true);
	bool GetGameInfo(int uid, Json::Value& info);
	bool MergeJson(Json::Value& jvRet, const Json::Value& jvMerge1, const Json::Value& jvMerge2);
	bool MergeJson(Json::Value& jvRet, const Json::Value& jvMerge);

	int feedBack(int gameid, int cid, int sid, int pid, int ctype, const char* content, int mid,
		const char* nick, const char* phoneno, const char* pic, const char* ip, int status=0, int productid=0);
	bool setLoginRecord(const Json::Value& jvUser);

	bool getPasswordByphoneNo(Json::Value& jvRet, std::string strPhoneNo, int32_t nProductId = 1);
	bool bindingAlipay(const Json::Value& jvAliInfo) ;

	int addLogin(Json::Value& user);
}

