#include "TransCreateRoomProc.h"
#include "ServerManager.h"
#include "ErrorMsg.h"
#include "Logger.h"
#include "BackConnect.h"

TransCreateRoomProc::TransCreateRoomProc()
{
	this->name = "TransCreateRoomProc";
}

TransCreateRoomProc::~TransCreateRoomProc()
{
}

int TransCreateRoomProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(pt);

	int cmd = inputPacket->GetCmdType();
	short seq = inputPacket->GetSeqNum();
	BYTE gameID = inputPacket->GetOptType();
	// 加入类型
	BYTE joinType = inputPacket->ReadByte();
	// 游戏等级
	short gameLevel = inputPacket->ReadShort();
	// 玩家ID
	int uid = inputPacket->ReadInt();
	// 参数
	string jsonOptions = inputPacket->ReadString();
	// 大厅ID (hall服务器拼接的)
	int hallID = inputPacket->ReadInt();

	ULOGGER(E_LOG_DEBUG, uid) << "cmd = " << TOHEX(cmd)
		<< " seq = " << seq
		<< " gameID = " << gameID
		<< " joinType = " << joinType
		<< " gameLevel = " << gameLevel
		<< " uid =  " << uid
		<< " jsonOptions = " << jsonOptions
		<< " hallID = " << hallID;

	// seq: 客户端和大厅的标识号
	// pt->seq: 服务器内部处理用的标识号
	inputPacket->SetSeqNum(pt->seq);
	pt->seq = seq;

	// From hall, to UserServer
	if (BackConnectManager::getNodes(uid % 10 + 1)->send(inputPacket) < 0)
	{
		ULOGGER(E_LOG_DEBUG, uid) << "Transfer msg(cmd = " << cmd << ") to User server failed!";
		return -1;
	}
	else
	{
		ULOGGER(E_LOG_DEBUG, uid) << "Transfer msg(cmd = " << cmd << ") to User server success!";
		pt->client = clientHandler;
		return 1;
	}

	return 0;
}

int TransCreateRoomProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket, Context* pt)
{
	_NOTUSED(clientHandler);
	if (pt == NULL || pt->client == NULL)
	{
		// context转发出错
		LOGGER(E_LOG_ERROR) << "Context is transfer to alloc occurs error!";
		return -1;
	}

	ClientHandler *pCaller = reinterpret_cast<ClientHandler*>(pt->client);

	short cmd = inputPacket->GetCmdType();
	short seq = inputPacket->GetSeqNum();
	short gameID = inputPacket->GetOptType();
	// US返回码
	short retCode = inputPacket->ReadShort();
	// US返回信息
	string retMsg = inputPacket->ReadString();
	// 桌子ID
	int packedTableID = inputPacket->ReadInt();
	// 加入类型
	BYTE joinType = inputPacket->ReadByte();
	// 游戏等级
	short gameLevel = inputPacket->ReadShort();
	// 玩家ID
	int uid = inputPacket->ReadInt();
	// 参数
	string jsonOptions = inputPacket->ReadString();
	// 大厅ID (hall服务器拼接的)
	int hallID = inputPacket->ReadInt();

	ULOGGER(E_LOG_DEBUG, uid) << "cmd = " << TOHEX(cmd)
		<< " seq = " << seq
		<< " gameID = " << gameID
		<< " retCode = " << retCode
		<< " retMsg = " << retMsg
		<< " packedTableID = " << packedTableID
		<< " joinType = " << joinType
		<< " gameLevel = " << gameLevel
		<< " uid = " << uid
		<< " jsonOptions = " << jsonOptions
		<< " hallID = " << hallID;


	/////////////////////////////////////////////////////////////////////////////////
	// 在Alloc可以处理的

	if (retCode == -6)
	{
		// 用户不存在
		return sendErrorMsg(pCaller, cmd, uid, -6, retMsg.c_str(), seq);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// 需要转发到Game进行处理的

	CDLSocketHandler *pGameServer = ServerManager::getInstance()->getGameServerHandlerByLevel(gameLevel, gameID);
	if (NULL == pGameServer)
	{
		// 找不到游戏服务器
		return sendErrorMsg(pCaller, cmd, uid, -2, ERRMSG(-2), seq);
	}

	// From UserServer, to GameServer
	OutputPacket response;
	response.Copy(inputPacket->packet_buf(), inputPacket->packet_size());
	response.SetSeqNum(pt->seq);	// 还原为“客户端和大厅的标识号”
	response.End();
	if (ServerManager::getInstance()->send(pGameServer, &response, false) < 0)
	{
		// 匹配失败
		return sendErrorMsg(pCaller, cmd, uid, -7, ERRMSG(-7), seq);
	}

	return 0;
}

