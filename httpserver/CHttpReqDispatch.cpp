#include "CHttpReqDispatch.h"
#include <stdlib.h>
#include <string.h>
#include "json/json.h"
#include "Logger.h" 
#include "CalUsingTime.h"
#include "url_code.h"
#include "base64.h"
//#if __DDCRYPTO__
//#include "DDCrypt.h"
#include "XyRsa.h"
#include "XyCryptoEx.h"
//#endif
#include "RedisLogic.h"
#include "Helper.h"
#include "Util.h"

#include "http_handle/CTest.h"
#include "http_handle/CGuestLogin.h"
#include "http_handle/CAccountLogin.h"
#include "http_handle/CRegUserName.h"
#include "http_handle/CRegUserNameBinding.h"
#include "http_handle/CRegGetLdCode.h"
#include "http_handle/CRegBindingAliPay.h"
#include "http_handle/CBankCreate.h"
#include "http_handle/CBankSaveMoney.h"
#include "http_handle/CBankGetMoney.h"
#include "http_handle/CBankResetPasswd.h"
#include "http_handle/CBankFindPasswd.h"
#include "http_handle/CUpdateUser.h"
#include "http_handle/CUpdateIcon.h"
#include "http_handle/CGetPayInfo.h"
#include "http_handle/CGetGameInfo.h"
#include "http_handle/CGetRank.h"
#include "http_handle/CGetRankAwardRules.h"
#include "http_handle/CGuestBinding.h"
#include "http_handle/CGetGameSwitch.h"
#include "http_handle/CGetRoomConfig.h"
#include "http_handle/CGetProductRoomConfig.h"
#include "http_handle/CGetGameVersion.h"
#include "http_handle/CGetGameOnline.h"
#include "http_handle/CSetDefaultIcon.h"
#include "http_handle/CGetGameResource.h"
#include "http_handle/CGetCheating.h"
#include "http_handle/CGetFeedBackSet.h"
#include "http_handle/CGetFeedBackHistory.h"
#include "http_handle/CGetTaskInfo.h"
#include "http_handle/CGetTaskAward.h"
#include "http_handle/CRegResetPassword.h"
#include "http_handle/CRegGetPassword.h"
#include "http_handle/CIssuedAccount.h"
#include "http_handle/CCheckIdCode.h"
#include "http_handle/CGetNotice.h"
#include "http_handle/CCheckMnick.h"
#include "http_handle/CCheckUserName.h"
#include "http_handle/CReceiveBenefits.h"
#include "http_handle/CReloadConfig.hpp"
#include "http_handle/CGetWinGoldRank.h"
#include "http_handle/CGetDrawGold.h"

const std::string TEST = "test";                              // 0. 测试接口
const std::string GUEST_LOGIN = "login.guest";                // 1.	快速登录
const std::string ACCOUNT_LOGIN = "login.account";              // 2.	账号登录 
const std::string REGISTER_USER_NAME = "register.username";          // 3.	账号注册
const std::string REGISTER_USER_NAME_BINDING = "register.usernamebinding";   // 4.	绑定手机号
const std::string REGISTER_GET_LD_CODE = "register.getidcode";         // 5.	获取验证码
const std::string REGISTER_BINDING_ALI_PAY = "register.bindingalipay";     // 6.	绑定支付宝

const std::string BANK_CREATE = "bank.create";                // 7.	创建保险箱
const std::string BANK_SAVE_MONEY = "bank.savemoney";             // 8.	保险箱存钱
const std::string BANK_GET_MONEY = "bank.getmoney";              // 9.	保险箱取钱
const std::string BANK_RESET_PASSWORD = "bank.resetpassword";         // 10.	保险箱重置密码
const std::string BANK_FIND_PASSWORD = "bank.findpassword";          // 11.	通过手机验证码重置保险箱密码

const std::string CLIENT_BASE_UPDATE_USER = "clientbase.updateuser";      // 12.	更新用户信息
const std::string CLIENT_BASE_UPDATE_ICON = "clientbase.uploadicon";      // 13.	更新用户头像
// 14.	更新用户头像

const std::string CLIENT_BASE_GET_PAY_INFO = "clientbase.getpayinfo";          // 15.	客户短获取订单信息弹框 Clientbase 是正确的吗?
const std::string CLIENT_BASE_GET_GAME_INFO = "clientbase.getgameinfo";         // 16.	客户端获取游戏金币信息
const std::string CLIENT_BASE_GET_RANK = "clientbase.getrank";						   // 17.	获取排行榜
const std::string CLIENT_BASE_GET_RANK_AWARD_RULES = "clientbase.getrankawardrules";   // 18.	获取排行榜规则
const std::string CLIENT_BASE_GUEST_BING_DING = "clientbase.guestbingding";       // 19.	游客帐号升级
const std::string CLIENT_BASE_GET_GAME_SWITCH = "clientbase.getgameswitch";       // 20.	获取支付方式等开关
const std::string CLIENT_BASE_GET_ROOM_CONFIG = "clientbase.getroomconfig";       // 21.	获取游戏房间配置，老接口
const std::string CLIENT_BASE_GET_PRODUCT_ROOM_CONFIG = "clientbase.getproductroomconfig";    // 22.	获取游戏房间配置，新接口

const std::string CLIENT_BASE_GET_GAME_VERSION = "clientbase.getgameversion";      // 23.	获取游戏最新版本，用于更新
const std::string CLIENT_BASE_GET_GAME_ONLINE = "clientbase.getgameonline";       // 24.	获取游戏在线人数
const std::string CLIENT_BASE_SET_DEFAULT_ICON = "clientbase.setdefaulticon";      // 25.	设置默认头像
const std::string CLIENT_BASE_GET_GAME_RESOURCE = "clientbase.getgameresource";     // 26.	获取资源，用于热更
const std::string CLIENT_BASE_GET_CHEATING = "clientbase.getcheating";         // 27.	斗地主举报
const std::string FEEDBACK_SET = "feedback.set";                   // 28.	用户提交反馈
const std::string FEEDBACK_GETHISTORY = "feedback.gethistory";     // 29.	用户获取反馈历史列表
const std::string GET_TASK_INFO = "task.gettaskinfo";                    // 30.	获取任务列表
const std::string GET_TASK_AWARD = "task.gettaskaward";                   // 31.	领取任务奖励
const std::string GET_NOTICE  = "clientbase.getnotice";               //32. 获取公告
const std::string CHECK_MNICK = "clientbase.checkmnick";              //33. 检查昵称
const std::string RECEIVE_BENEFITS = "clientbase.setmoney";              //34. 领取救济金
const std::string CHECK_USERNAME = "register.checkusername";              //35.检测用户名

const std::string REGISTER_RESEET_PASSWORD = "register.resetpassword";   // 36.	重置账号密码
const std::string REGISTER_GET_PASSWORD = "register.getpassword";		// 通过短信获取密码

const std::string CHECK_ID_CODE = "register.checkidcode";           // 37. 检查短信验证码是否正确
const std::string REGISTER_ISSUED_ACCOUNT = "register.issuedaccount"; // 38. 生成随机账号

const std::string ADMIN_RELOAD_CONFIG = "admin.reloadconfig";           // 39 管理操作，重新加载配置文件
const std::string CLIENT_BASE_WINGOLD_RANK = "clientbase.getwingoldrank";    // 40 赢金币获得排行榜
const std::string CLIENT_BASE_DRAW_GOLD = "clientbase.getdrawgold";    // 41 抽金币(老虎机)

#if __DDCRYPTO__

const std::string g_strDefAuthKeyOptionArr[] = {TEST, GUEST_LOGIN, ACCOUNT_LOGIN, REGISTER_USER_NAME, REGISTER_ISSUED_ACCOUNT, REGISTER_GET_PASSWORD, CLIENT_BASE_GET_GAME_SWITCH, 
	CLIENT_BASE_GET_GAME_VERSION, CLIENT_BASE_GET_PRODUCT_ROOM_CONFIG, CLIENT_BASE_GET_GAME_RESOURCE, CLIENT_BASE_GET_GAME_INFO, CLIENT_BASE_GET_PAY_INFO, GET_NOTICE,
	""};

bool DefAuthKeyOption(const std::string& strMethod)
{
	int i = 0;
	while (!g_strDefAuthKeyOptionArr[i].empty())
	{
		if (g_strDefAuthKeyOptionArr[i] == strMethod) return true;
		i++;
	}
	return false;
}

unsigned char privateKey [] = "-----BEGIN RSA PRIVATE KEY-----\n"\
	"MIICWgIBAAKBgFPZr4m25ssVIY/YSKdaG/I/NfXkmzPI51mSJyrnwbzFl/zyQxH2\n"\
	"yvRxeyVYjvBA0hQnJ7LpyxgR/sxyjsHSAg93hyNEAw4MBhKZ0dDRAqKmyrUs6Thw\n"\
	"jLQpw0OmNw3Ca+yorggqvX5oReEiSyk46u7ciay3iyERBy60tbWihhNtAgMBAAEC\n"\
	"gYAQf+Vq0/LWQ3I3O/gP9ktJf+a+XB2uMvpTRc8vQit9WeRugb1w639EWpBA6Qw3\n"\
	"eoncLVhCfH7aXbAlyuUMfqao6tM9f1rAIBFKfDknjGRX8N9+r+zo82fspV1TLMAz\n"\
	"cth+asjPRk0Oyky23rOSCg3xNFAu6CjYsqMGRQ2ZN8jEAQJBAJKbtBPNKdbm0baH\n"\
	"s75h/qqa41CHaQB1bu9Zvoy1hszZVe73sF5Hp10gTjRR7AUJxXZibVXObFcgpbXV\n"\
	"ke8i8ZUCQQCSalD2jtWzMDICWAONoI33RmEeN2QTpY/sFIvjIFUIZIJVM5oyzlq1\n"\
	"eqwcxhaAG7KQr8JkcRiMP5vkb7yviVR5AkA1Wvs3daQzdL9/yXVN5UYUetgdl5pM\n"\
	"M3DTJPsnJG1RogsXNAd42GT9jGNJwUK/NqYphnq6Dqz5LIWCXp6ExFfdAkAWkisv\n"\
	"By/scraS4+yQTbr07rWUCef0m2ZHd5dlCRvyskPhTJYt1N/o8CNOQD9BuoNZiK7H\n"\
	"+yNUo42ttof464vJAkAerD4kH38LX4JnhUa9wnZRbmeisTGEKZEtuKwcxzotQi3L\n"\
	"Isd0bMA63NtXE6PzFY25C+aaK4WUDE/pXo791l5g\n"\
	"-----END RSA PRIVATE KEY-----";

#endif

CHttpReqDispatch::~CHttpReqDispatch()
{
	std::map<std::string, CHttpRequestHandler*>::iterator itTmp = m_handlers.begin();
	for (;itTmp != m_handlers.end();itTmp++)
	{
		if (itTmp->second) delete itTmp->second;
//		m_handlers.erase(itTmp);
	}
	m_handlers.clear();
}

void CHttpReqDispatch::register_handler()
{
    m_handlers[TEST] = new CTest();
    m_handlers[GUEST_LOGIN] = new CGuestLogin();
    m_handlers[ACCOUNT_LOGIN] = new CAccountLogin();
    m_handlers[REGISTER_USER_NAME] = new CRegUserName();
    m_handlers[REGISTER_USER_NAME_BINDING] = new CRegUserNameBinding();
    m_handlers[REGISTER_GET_LD_CODE] = new CRegGetLdCode();
    m_handlers[REGISTER_BINDING_ALI_PAY] = new CRegBindingAliPay();
    m_handlers[BANK_CREATE] = new CBankCreate();
    m_handlers[BANK_SAVE_MONEY] = new CBankSaveMoney();
    m_handlers[BANK_GET_MONEY] = new CBankGetMoney();
    m_handlers[BANK_RESET_PASSWORD] = new CBankResetPasswd();
    m_handlers[BANK_FIND_PASSWORD] = new CBankFindPasswd();
    m_handlers[CLIENT_BASE_UPDATE_USER] = new CUpdateUser();
    m_handlers[CLIENT_BASE_UPDATE_ICON] = new CUpdateIcon();
    m_handlers[CLIENT_BASE_GET_PAY_INFO] = new CGetPayInfo();
    m_handlers[CLIENT_BASE_GET_GAME_INFO] = new CGetGameInfo();
    m_handlers[CLIENT_BASE_GET_RANK] = new CGetRank();
    m_handlers[CLIENT_BASE_GET_RANK_AWARD_RULES] = new CGetRankAwardRules();
    m_handlers[CLIENT_BASE_GUEST_BING_DING] = new CGuestBinding();
    m_handlers[CLIENT_BASE_GET_GAME_SWITCH] = new CGetGameSwitch();
    m_handlers[CLIENT_BASE_GET_ROOM_CONFIG] = new CGetRoomConfig();
    m_handlers[CLIENT_BASE_GET_PRODUCT_ROOM_CONFIG] = new CGetProductRoomConfig();
    m_handlers[CLIENT_BASE_GET_GAME_VERSION] = new CGetGameVersion();
    m_handlers[CLIENT_BASE_GET_GAME_ONLINE] = new CGetGameOnline();
    m_handlers[CLIENT_BASE_SET_DEFAULT_ICON] = new CSetDefaultIcon();
    m_handlers[CLIENT_BASE_GET_GAME_RESOURCE] = new CGetGameResource();
    m_handlers[CLIENT_BASE_GET_CHEATING] = new CGetCheating();
    m_handlers[FEEDBACK_SET] = new CGetFeedBackSet();
    m_handlers[FEEDBACK_GETHISTORY] = new CGetFeedBackHistory();
    m_handlers[GET_TASK_INFO] = new CGetTaskInfo();
	m_handlers[GET_TASK_AWARD] = new CGetTaskAward();
	m_handlers[REGISTER_RESEET_PASSWORD] = new CRegResetPassword();
	m_handlers[REGISTER_GET_PASSWORD] = new CRegGetPassword();
    m_handlers[REGISTER_ISSUED_ACCOUNT] = new CIssuedAccount();
    m_handlers[CHECK_ID_CODE] = new CCheckIdCode();
    m_handlers[GET_NOTICE]    = new CGetNotice();
    m_handlers[CHECK_MNICK]   = new CCheckMnick();
    m_handlers[CHECK_USERNAME] = new CCheckUserName();
    m_handlers[RECEIVE_BENEFITS] = new CReceiveBenefits();
    m_handlers[ADMIN_RELOAD_CONFIG] = new CReloadConfig();
	m_handlers[CLIENT_BASE_WINGOLD_RANK] = new CGetWinGoldRank();
	m_handlers[CLIENT_BASE_DRAW_GOLD] = new CGetDrawGold();
}

CHttpRequestHandler* CHttpReqDispatch::getHttpRequestHandler(const std::string& method)
{
	std::map<std::string, CHttpRequestHandler*>::iterator it = m_handlers.find(method);
	if (it == m_handlers.end())
	{
		return NULL;
	}
	
	return it->second;
}


int CHttpReqDispatch::DispatchReq(const char* recvbuf, unsigned int rcvlen, const char* uri, const char *client_ip, std::string& strResult)
{
    if (uri)
    {
        _LOG_DEBUG_("Http req dispatch: %s", uri);
    }
    if (!recvbuf || rcvlen == 0 || !uri || !client_ip )
    {
        _LOG_WARN_("Http req dispatch: rcvlen[%u]\n", rcvlen);
        return -1;
    }
	//解密
#if __DDCRYPTO__

	string strMsg = XyDeCrypt((unsigned char *)recvbuf,rcvlen);
	if (strMsg.empty() || strMsg.length() < 10)
	{
		_LOG_WARN_("Http req dispatch: rcvlen[%u] is error\n", rcvlen);
		return -1;
	}

	std::string strApi(strMsg.c_str(),4);

	
#else
	if (rcvlen < 10)
	{
		_LOG_WARN_("Http req dispatch: rcvlen[%u] is too short\n", rcvlen);
		return -1;
	}
	std::string strApi(recvbuf,4);
#endif
	//end 解密

    //_LOG_INFO_("[HttpSvr:handleReq] recv buffer is[%s]\n", recvbuf);
    //LOGGER(E_LOG_INFO) << "recv buffer is " << recvbuf;
//	strApi.assign(recvbuf, 4);
	ToLower(strApi);
	if (strApi != "api=")
	{
		_LOG_WARN_("Http req dispatch: error key for post[%s]\n", strApi.c_str());
	     return -1;
	}

    try
    {
        Json::Reader jReader(Json::Features::strictMode());
        Json::Value jvRoot;
        std::string strJson;
#if __DDCRYPTO__
		strJson.assign(strMsg.begin() + 4, strMsg.end());
#else
		strJson.assign(recvbuf + 4, rcvlen - 4);
#endif   
		strJson = urldecode(strJson);
		LOGGER(E_LOG_DEBUG) << "Recv Msg From: " << client_ip << "; Json is :" << strJson;

        if (!jReader.parse(strJson, jvRoot))
        {
            _LOG_WARN_("[HttpSvr:handleReq] recv error json[%s]\n", strJson.c_str());
            return -1;
        }

		std::string method = jvRoot["param"].get("method", "").asString();
		//LOGGER(E_LOG_INFO) << "method is :" << method;
		ToLower(method);

	    CHttpRequestHandler* handler = getHttpRequestHandler(method);
		int ret = status_method_error;
		HttpResult out;
	    if (handler)
	    {
			CCalUsingTime calUsingTime(method);
            ret = handler->do_request(jvRoot, (char *)client_ip, out);
            handler->makeRet(ret, out);
	    }
	    else
		{
			_LOG_INFO_("method %s miss match the request handler !!!!!!", method.c_str());
			out["result"] = status_method_error;
			out["msg"] = "方法错误";
		}
		Json::FastWriter jWriter;
		//加密
#if __DDCRYPTO__

		std::string strSrcJson = jWriter.write(out);
		strResult = XyEnCrypt((unsigned char *)strSrcJson.c_str(),strSrcJson.length());
		if(strResult.empty())
		{
			_LOG_INFO_("method %s [Private Encrypt failed] !!!!!!", method.c_str());
			return -1;
		}
		/*unsigned char RetEncrypted[40960] =  {0};
		encrypted_length = private_encrypt((unsigned char *)strSrcJson.c_str(), strSrcJson.size(), privateKey, RetEncrypted);
		if (encrypted_length == -1)
		{
			_LOG_INFO_("method %s [Private Encrypt failed] !!!!!!", method.c_str());
			return -1;
		}
		char*  RetB64Encrypted = NULL;
		encrypted_length = base64::base64Encode(RetEncrypted,encrypted_length,&RetB64Encrypted);
		if (RetB64Encrypted) 
		{
			strResult = RetB64Encrypted;
			free(RetB64Encrypted);
			RetB64Encrypted = NULL;
		}
		else
		{
			_LOG_INFO_("method %s [Private Encrypt failed2] !!!!!!", method.c_str());
			return -1;
		}*/
#else
		strResult = jWriter.write(out);
#endif
		return strResult.size();
    }
    catch (std::exception &ex)
    {
	    _LOG_ERROR_("exception occured !!!%s", ex.what());
    }

    return 0;
}

