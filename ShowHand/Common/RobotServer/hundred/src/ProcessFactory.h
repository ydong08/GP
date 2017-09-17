#ifndef _ProcessFactory_H_
#define _ProcessFactory_H_

#include "IProcess.h"

class ProcessFactory  
{
	public:
		ProcessFactory(){};
		virtual ~ProcessFactory(){};
		static IProcess* getProcesser(int cmd); 
	private:
		static IProcess* createProcesser(int cmd);
	   
};
#endif



