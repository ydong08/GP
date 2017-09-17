

#include "MySqlLogic.h"
#include "MySqlAgent.h"
#include "RedisAccess.h"
#include "StrFunc.h"
#include "RedisLogic.h"
#include "ProtocolServerId.h"
#include "Packet.h"
#include "Logger.h"
#include "JsonValueConvert.h"
#include "Utility.h"
#include "Configure.h"

#include "json/json.h"

namespace MySqlLogic
{
    std::string getPlayerInfo(const std::vector<CardRoomPlayerInfo> &history) {
        std::string sInfo = "[]";
        if (history.size() > 0) {
            Json::Value playerinfo;
            for (auto it = history.begin(); it != history.end(); it++)
            {
                const auto& info = *it;
                Json::Value pi;
                pi["uid"] = info.uid;
                pi["index"] = info.index;
                pi["name"] = info.name;
                pi["score"] = info.score;
                playerinfo.append(pi);
            }
            sInfo = Utility::DumpJson(playerinfo);
        }
        return sInfo;
    }

	bool addCardRoomHistory(MySqlAgent *mysql, int roomowner, int tid, int gameid, int totalround, int createtime,
                            int cost, int playercount)
	{
		OutputPacket msg;
		msg.Begin(S2S_MYSQL_ADD_CARDROOM_HISTORY);
		msg.WriteInt(roomowner);
		msg.WriteInt(gameid);
		msg.WriteInt(tid);
		msg.WriteInt(0); //finish round
		msg.WriteInt(totalround);
		msg.WriteInt(cost);
		msg.WriteInt(0); //status
		Json::Value playerinfo;
		for (int i = 1; i < playercount; i++)
		{
			Json::Value pi;
			pi["uid"] = 0;
            pi["index"] = i;
			pi["name"] = "";
			pi["score"] = 0;
			playerinfo.append(pi);
		}
		// 将创建者添加进去
		Json::Value pi;
		pi["uid"] = roomowner;
        pi["index"] = 0;
		pi["name"] = "";
		pi["score"] = 0;
		playerinfo.append(pi);
		msg.WriteString(Utility::DumpJson(playerinfo));
		msg.WriteInt(createtime);
		msg.WriteInt(1);		// 表示添加表
		msg.End();
		return mysql->Send(&msg) >= 0;
	}

	bool updateCardRoomHistory(MySqlAgent *mysql, int roomowner, int tid, int gameid,  int createtime,
                               int finishround, int status, const std::vector<CardRoomPlayerInfo> &history)
	{
		OutputPacket msg;
		msg.Begin(S2S_MYSQL_ADD_CARDROOM_HISTORY);
		msg.WriteInt(roomowner);
		msg.WriteInt(gameid);
		msg.WriteInt(tid);
		msg.WriteInt(finishround);
		msg.WriteInt(0); //totalround
		msg.WriteInt(0); //cost
        msg.WriteInt(status);
        std::string sInfo = getPlayerInfo(history);

		msg.WriteString(sInfo);
		msg.WriteInt(createtime);
		msg.WriteInt(0);		// 表示更新表
		msg.End();
		return mysql->Send(&msg) >= 0;
	}

    bool addCardRoomRecord(MySqlAgent *mysql, int roomowner, int tid, int gameid, int currentround, int totalround,
                           int createtime, int starttime, int endtime, int status, int playercount,
                           const std::vector<CardRoomPlayerInfo> &history, const std::string& playinfo)
    {
        OutputPacket msg;
        msg.Begin(S2S_MYSQL_ADD_CARDROOM_RECORD);
        msg.WriteInt(roomowner);
        msg.WriteInt(gameid);
        msg.WriteInt(tid);
        msg.WriteInt(createtime);
        msg.WriteInt(starttime);
        msg.WriteInt(endtime);
        msg.WriteInt(currentround);
        msg.WriteInt(totalround);
        std::string sInfo = getPlayerInfo(history);

        msg.WriteString(sInfo);
        msg.WriteString(playinfo);
        msg.End();
        return mysql->Send(&msg) >= 0;
    }

	bool addRoomCardChargeLog(MySqlAgent *mysql, int roomowner, int tid, int gameid, int pid, int64_t winMoney,
							  int64_t currentMoney) {
		OutputPacket outputPacket;
		outputPacket.Begin(0x120);
		outputPacket.WriteInt(roomowner);
		outputPacket.WriteInt64(winMoney);
		outputPacket.WriteInt(0);
		outputPacket.WriteInt(0);
		outputPacket.WriteInt(0);
		outputPacket.WriteInt(pid);
		outputPacket.WriteInt(0);  //tax
		outputPacket.WriteInt(0);  //ante
		outputPacket.WriteInt(Configure::getInstance()->m_nServerId);
		outputPacket.WriteInt(tid);
		outputPacket.WriteInt(0);
		outputPacket.WriteString("");
		outputPacket.WriteInt(0); //0 表示正常结束， 1表示掉线
		outputPacket.WriteInt64(currentMoney);
		outputPacket.WriteInt(time(NULL));
		outputPacket.WriteInt(time(NULL));
		outputPacket.WriteInt(0);
		outputPacket.End();
		return mysql->Send(&outputPacket) >= 0;
	}
}

