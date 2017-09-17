#ifndef SLOCKER_HEAD
#define SLOCKER_HEAD
#include <mutex>
class SLocker
{
public:
	SLocker(std::mutex &mutex) : _mutex(mutex)
	{
		_mutex.lock();
	}

	virtual ~SLocker()
	{
		_mutex.unlock();
	}


	void lock()
	{
		return _mutex.lock();
	}


	void unlock()
	{
		return _mutex.unlock();
	}

private:
	std::mutex &_mutex;
};
#endif
