
#include "NoteLeaveListProc.h"

int NoteLeaveListProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int NoteLeaveListProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;
	int uid = inputPacket->ReadInt(); 
	printf("Data Recv: uid=[%d] \n", uid);
	short pstatus = inputPacket->ReadShort();
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	printf("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	printf("Data Recv: tstatus=[%d] \n", tstatus);
	int bankerid = inputPacket->ReadInt();
	printf("Data Recv: bankerid=[%d] \n", bankerid);
	printf("Data Recv: bankerseatid=[%d] \n", inputPacket->ReadShort());
	return 0;
	//return EXIT;

}

