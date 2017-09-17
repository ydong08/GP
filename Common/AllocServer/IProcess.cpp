#include "IProcess.h"
#include "Logger.h"


int IProcess::send(CDLSocketHandler* clientHandler, OutputPacket* outPacket, bool isEncrypt)
{
	if(clientHandler && outPacket)
	{
		if(isEncrypt)
			outPacket->EncryptBuffer();
		return clientHandler->Send((const char*)outPacket->packet_buf(), outPacket->packet_size()) ;
	}
	return -1;	 
}

int IProcess::sendErrorMsg(ClientHandler* clientHandler,int cmd, int uid , short errcode, const char* errmsg, unsigned short seq)
{
	OutputPacket response;
	response.Begin(cmd,uid);
	response.SetSeqNum(seq);
	response.WriteShort(errcode);
	response.WriteString(errmsg);
	response.WriteInt(uid);
	response.End();
	ULOGGER(E_LOG_WARNING, uid) << "[DATA Reponse] erron= " << errcode << " errmsg = " << errmsg;
	if (clientHandler)
	{
		clientHandler->Send((const char*)response.packet_buf(), response.packet_size());
	}
	return 0;
}
