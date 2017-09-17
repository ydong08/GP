#include "RequestDissolveRoom.h"
#include "ProcessManager.h"
#include "HallHandler.h"
#include "Logger.h"
#include "ErrorMsg.h"
#include "GameCmd.h"

// byte option (0-不同意; 1-同意)
const int CLIENT_MSG_DISSOLVE_REQUEST	= 0x0232;
const int CLIENT_MSG_DISSOLVE_RESPONSE	= 0x0233;

REGISTER_PROCESS(CLIENT_MSG_DISSOLVE_REQUEST, RequestDissolveRoom)

RequestDissolveRoom::RequestDissolveRoom()
{
	this->name = "RequestDissolveRoom";
}

RequestDissolveRoom::~RequestDissolveRoom()
{
}

int RequestDissolveRoom::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(pt);

	int cmd = inputPacket->GetCmdType();
	short seq = inputPacket->GetSeqNum();
	BYTE gameID = inputPacket->GetOptType();
    short source = inputPacket->GetSource();
    short retno = inputPacket->ReadShort();
    std::string retmsg = inputPacket->ReadString();
    int uid = inputPacket->ReadInt();
    int req_uid = inputPacket->ReadInt();
    // 房间号
    int tid = inputPacket->ReadInt();
	short realTid = tid & 0xFFFF;

	ULOGGER(E_LOG_INFO, uid)
		<< " cmd = " << cmd
		<< " gameID = " << gameID
		<< " source = " << source
		<< " uid = " << uid
		<< " tid = " << tid
		<< " realTid = " << realTid;

    HallHandler *hallhandler = reinterpret_cast<HallHandler*> (clientHandler);
	int retCode = 0;

	OutputPacket response;
	response.Begin(CLIENT_MSG_DISSOLVE_RESPONSE);
	response.WriteInt(uid);
	response.WriteInt(tid);
	response.WriteByte(1);
	response.End();

	this->send(hallhandler, &response);

	return 0;
}

int RequestDissolveRoom::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
    LOGGER(E_LOG_INFO) << "test";
	return 0;
}

