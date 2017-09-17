#ifndef __ProcessManager_H_
#define __ProcessManager_H_

#include <map>
#include "Typedef.h"
#include "BaseProcess.h"

class ProcessManager
{	
	typedef std::map<int, BaseProcess*> MapProcess;

	Singleton(ProcessManager);

public:
	
	BaseProcess* getProcesser(int cmd); 

	bool addProcesser(int cmd, BaseProcess* process);

	bool addTransfProcesser(int startCmd, int endCmd, BaseProcess *process);

private:	
	MapProcess  m_mapProcess;
};

#define REGISTER_PROCESS(Cmd, ClassName) \
	bool add_##Cmd##_##ClassName = ProcessManager::getInstance().addProcesser(Cmd, new ClassName());

#define REGISTER_TRANSF_PROCESS(startCmd, endCmd, ClassName) \
	bool add_##ClassName = ProcessManager::getInstance().addTransfProcesser(startCmd, endCmd, new ClassName());

//get cmd process.
#define GET_PROCESS(cmd)  ProcessManager::getInstance().getProcesser(cmd)

#endif



