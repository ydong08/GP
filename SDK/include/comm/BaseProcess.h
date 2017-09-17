#ifndef __BASE_PROCESS_H__
#define __BASE_PROCESS_H__

#include <string>
#include "CDLSocketHandler.h"
#include "Packet.h"

#define _NOTUSED(V) ((void) V)

class BaseProcess;

typedef struct Context {
    int netfd;
    unsigned int seq;
    char status;
    PacketHeader msgHeader;
    BaseProcess *process;
    CDLSocketHandler *client;
    void *data;
} Context;

class BaseProcess {
public:
    BaseProcess(bool encrypt = false) {
        bEncrypt = encrypt;
    };

    virtual ~BaseProcess() {

    };

    virtual int doRequest(CDLSocketHandler *handler, InputPacket *inputPacket, Context *pt) { return 0; }

    virtual int doResponse(CDLSocketHandler *handler, InputPacket *inputPacket, Context *pt) { return 0; }

    bool isEncryptCmd() const { return bEncrypt; }
	//NOTE:仅用于新版加密大厅，其余地方不应该调用该操作
	void setEncryptCmd(bool encrypt) { bEncrypt = encrypt; }

public:
    std::string name;
    short cmd;

protected:
	int sendErrorMsg(CDLSocketHandler *handler, int cmd, int uid, short errcode, unsigned short seq = 0);

    int sendErrorMsg(CDLSocketHandler *handler, int cmd, int uid, short errcode, const char *errmsg,
                     unsigned short seq = 0);

    int send(CDLSocketHandler *handler, OutputPacket *outPack, bool isEncrypt = true);

private:
    bool bEncrypt;
};

#endif
