#ifndef __TIMERLIST_H__
#define __TIMERLIST_H__

#include <list.h>
#include <timestamp.h>

class CTimerObject;
class CTimerUnit;

class CTimerList {
private:
	CListObject<CTimerObject> tlist;
	int timeout;
	CTimerList *next;

public:
	friend class CTimerUnit;
	friend class CTimerObject;
	CTimerList(int t) : timeout(t), next(NULL) { }
	~CTimerList(void) { tlist.FreeList(); }
	int CheckExpired(int64_t now=0);
};

class CTimerUnit {
private:
	CTimerList pending;
	CTimerList *next;
public:
	friend class CTimerObject;
	CTimerUnit(void);
	~CTimerUnit(void);

	CTimerList *GetTimerList(int);
	int ExpireMicroSeconds(int);
	int CheckExpired(int64_t now=0);
	int CheckPending(void);
};

class CTimerObject: private CListObject<CTimerObject> {
private:
	int64_t objexp;

public:
	friend class CTimerList;
	friend class CTimerUnit;
	CTimerObject() { }
	virtual ~CTimerObject(void);
	virtual void TimerNotify(void);
	void DisableTimer(void) { ResetList(); }
	void AttachTimer(class CTimerList *o);
	void AttachZeroTimer(class CTimerUnit *o) { ListMoveTail(o->pending.tlist); }
};

#endif
