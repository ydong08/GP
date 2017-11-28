#include "CGetCheating.h"
#include "json/json.h"
#include "Logger.h"
#include "threadres.h"
#include "Util.h"

const std::string strTb_Cheat                = "zq_admin.cheat";


int CGetCheating::do_request(const Json::Value& root, char *client_ip, HttpResult& out)
{
    //Json::Value ret;
    //Json::FastWriter jWriter;
    int productid, gameid, informantid, table_id, game_board;
    string mids;
    string content;
    try 
	{
        productid= Json::SafeConverToInt32(root["productid"]);
        gameid= Json::SafeConverToInt32(root["gameid"]);
        informantid= Json::SafeConverToInt32(root["mid"]);
        table_id = Json::SafeConverToInt32(root["param"]["table_id"]);
        game_board= Json::SafeConverToInt32(root["param"]["game_board"]);
        mids  = root["param"]["mids"].asString();
        content = root["param"]["content"].asString();
    } 
	catch (...)
	{
        out["status"] = 0;
        out["msg"] = "参数不正确";
        //out = jWriter.write(ret);
        return status_param_error;
    }
	if (informantid <0 || gameid < 0 || productid < 0 || root["productid"].isNull() || root["gameid"].isNull() || root["mid"].isNull())
	{
		out["status"] = 0;
		out["msg"] = "参数不正确";
		//out = jWriter.write(ret);
		return status_ok;
	}

    out["status"] = 0;
    if (table_id < 0) 
	{
        out["msg"] = "桌子id为空";
        //out = jWriter.write(ret);
        return status_ok;
    }

  //  string sql = StrFormatA("update %s set productid=%d,gameid=%d,table_id=%d,informantid=%d,game_board=%d,mids='%s',created_at='%s'",
	//	strTb_Cheat.c_str(), productid, gameid, table_id, informantid, game_board, mids.c_str(), Util::formatGMTTimeS().c_str());
	string sql = StrFormatA("INSERT INTO %s (productid, gameid, table_id, informantid, game_board, mids, created_at, content, feedbackid, feedback_type, reuser) values(%d, %d, %d, %d, %d, '%s', '%s', '%s', %d, %d, '%s')",
		    strTb_Cheat.c_str(), 
            productid, 
            gameid, 
            table_id, 
            informantid, 
            game_board, 
            mids.c_str(), 
            Util::formatGMTTimeS().c_str(),
            content.c_str(),
            informantid,
            0,
            "");

    ThreadResource::getInstance().getDBConnMgr()->DBMaster()->ExeSQL(sql.c_str());

    out["status"] = 1;
    //out = jWriter.write(ret);
    //out = ret;
    return status_ok;
}
