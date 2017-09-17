#ifndef  _ICC_TIMER_HANDLE_H_
#define  _ICC_TIMER_HANDLE_H_

#include "timerlist.h"
#include "CDLReactor.h"

class CDL_Timer_Handler: public CTimerObject
{
	public:
		virtual void StartTimer(long interval)=0;
		virtual void StopTimer(void)=0;
	
	protected:
		virtual int ProcessOnTimerOut()=0;

};

class CCTimer: public CDL_Timer_Handler
{
	public:
		virtual void StartTimer(long interval)
		{
			this->_interval = interval;
			CDLReactor::Instance()->RegistTimer(this,interval);

		}
		virtual void StopTimer(void)
		{
			CTimerObject::DisableTimer();
		}

		virtual void TimerNotify(void)
		{
			ProcessOnTimerOut();
		}	
	protected:
		virtual int ProcessOnTimerOut()=0;

	private:
		long _interval;

};

#endif

