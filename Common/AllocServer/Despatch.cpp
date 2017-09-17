#include "Despatch.h"
#include "IProcess.h"
#include "Configure.h"
#include "Logger.h"
#include "ProcessFactory.h"
#include "GameCmd.h"

using namespace std;
static Despatch* instance = NULL;
#define MAX_CONTEXT 65536

Despatch* Despatch::getInstance()
{
	if(instance==NULL)
	{
		instance = new Despatch();
	}
	return instance;
}

Despatch::Despatch()
{
	seq = 0;
	context = (Context*) calloc(MAX_CONTEXT,sizeof(Context));
}

static void initContext(Context* pt , unsigned int seq, IProcess* processer, CDLSocketHandler* clientHandler, InputPacket* pPacket)
{
	pt->seq = seq;
	pt->status = 0;
	pt->process =processer;
	pt->client = clientHandler;
	
	if(pPacket)
		memcpy(&pt->msgHeader,pPacket->packet_buf(), sizeof(PacketHeader));
	else
		memset(&pt->msgHeader,0,sizeof(PacketHeader));
	if(pt->data)
	{
		free(pt->data);
		pt->data = NULL;
	}
}

int Despatch::doFrontRequest(CDLSocketHandler* clientHandler,  InputPacket* pPacket)
{
	short nCmdType = pPacket->GetCmdType();
	char Source = pPacket->GetSource();
	unsigned short sn = pPacket->GetSeqNum();

	LOGGER(E_LOG_DEBUG) << "==============WorkProcess[" << getpid() << "]:doFrontRequest =============";
	LOGGER(E_LOG_DEBUG) << "Recv Front Request, Cmd = " << nCmdType << " Source = " << Source << " sn = " << sn;

	if(nCmdType == GMSERVER_SET_STATUS || nCmdType == GMSERVER_ALL || nCmdType == GMSERVER_GET_INFO 
		|| nCmdType == GMSERVER_SET_INFO)
	{
		if (pPacket->CrevasseBuffer() == -1)
		{
			LOGGER(E_LOG_ERROR) << "[DATA CrevasseBuffer ERROR] data decrypt error";
			LOGGER(E_LOG_DEBUG) << "==============WorkProcess[" << getpid() << "]:CrevasseBuffer failed =============";
			return -1;
		}
	}

 	IProcess* processer = ProcessFactory::getProcesser(nCmdType);
	 
	if(processer)
	{
		_LOG_DEBUG_( "Prcesser [%s]\n" ,processer->name.c_str()) ;
		unsigned short seqNo = ++this->seq;
		if(seqNo==0)
			seqNo = ++this->seq;

		Context* pt = &context[seqNo];
		initContext( pt, seq, processer, clientHandler, pPacket );

		int ret = processer->doRequest( clientHandler, pPacket, pt);
		
		if(ret<0)
		{
			_LOG_ERROR_( "[doFrontRequest]  Process Error ret=[%d]\n" ,ret) ;
		}
		else if(ret>0)
		{				
			//clientHandler中添加回调任务序号，以备异常退出时清空回调任务
			//clientHandler->addCallbackSeq(seqNo);
			_LOG_DEBUG_("Send to BackServer,Wait for  BackServer Response. SN[%d]\n",seqNo);
		}
	}
	else
	{
		_LOG_ERROR_( "[doFrontRequest] Can't find Process ,cmd=[0x%04x]\n" ,nCmdType) ;
	}
	_LOG_DEBUG_("==============WorkProcess[%d]:doFrontRequest OK===========\n\n",getpid());
	 
	return 0;
}

int Despatch::doBackResponse(CDLSocketHandler* clientHandler, InputPacket* pPacket)
{
	short nCmdType = pPacket->GetCmdType();
	char Source = pPacket->GetSource();
	unsigned short sn = pPacket->GetSeqNum();

	_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse =============\n",getpid());
	_LOG_DEBUG_( "Recv Back Response, Cmd=[0x%04x] Source[%d] sn[%d]\n" ,nCmdType,Source,sn) ;
	if(sn==0)//
	{
		IProcess* processer = ProcessFactory::getProcesser(nCmdType);
		if(processer)
			processer->doResponse( clientHandler, pPacket, NULL);
		else
			_LOG_ERROR_( "[doBackResponse] Can't find Process ,cmd=[0x%04x]\n" ,nCmdType) ;
		return 0; 
	 }

	Context* pt = &context[sn];
	if(pt==NULL)
	{
		_LOG_ERROR_("Cann't find callback process whit cmd=[%d] seq=[%d]\n", nCmdType, sn);
		_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse OK===========\n\n",getpid());
		return 0;
	}

	if(pt->process==NULL &&	pt->client==NULL )
	{
		_LOG_ERROR_("CallBack Context has been clear, cmd=[%d] seq=[%d]\n", nCmdType, sn);
		_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse OK===========\n\n",getpid());
		return 0;
	}

	short ptCmd = htons(pt->msgHeader.main_cmd);

	if( ptCmd != nCmdType)
	{
		_LOG_ERROR_("CallBack Context cmd not equal nCmdType, cmd=[0x%04x] seq=[%d] nCmdType=[0x%04x]\n", ptCmd , sn, nCmdType);
		_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse OK===========\n\n",getpid());
		return 0;
	}
	int ptUid = htonl(pt->msgHeader.uid);

	if(ptUid != pPacket->GetUid())
	{
		_LOG_ERROR_("CallBack Context uid[%d] not equal pPacket Uid[%d], cmd=[0x%04x] seq=[%d] \n", ptUid, pPacket->GetUid(), nCmdType, sn);
		_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse OK===========\n\n",getpid());
		return 0;
	}

	IProcess * process = pt->process;
	_LOG_DEBUG_( "Find callback process , processer=[%s] cmd=[%d] seq=[%d]\n", process->name.c_str(), nCmdType,  pt->seq);

	int ret = process->doResponse(clientHandler, pPacket, pt);
	if(ret<0)
	{
		_LOG_ERROR_( "[doBackResponse]  Process Error ret=[%d]\n" ,ret) ;
		initContext( pt, seq, NULL, NULL, NULL );
		//清除clientHandler中的已完成的回调任务
		//	pt->client->deleteCallbackSeq(sn);
	}
	else if(ret==0)
	{
		initContext( pt, seq, NULL, NULL, NULL );
		//清除clientHandler中的已完成的回调任务
		//	pt->client->deleteCallbackSeq(sn);
	}
	 
	_LOG_DEBUG_("==============WorkProcess[%d]:doBackResponse OK===========\n\n",getpid());
	return 0;
}

void Despatch::deleteCallBackParm(int seq)
{
	Context* pt = &context[seq];
	initContext( pt, seq, NULL, NULL, NULL );
}


