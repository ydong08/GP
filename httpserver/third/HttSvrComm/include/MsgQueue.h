#ifndef __STDX_QUEUE_H
#define __STDX_QUEUE_H

// C++ 98 header files
#include <iostream>
#include <string>
#include <deque>
#include <thread>             // std::thread
#include <chrono>             // std::chrono::seconds
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable>

namespace stdx{

template <typename T>
class msg_queue
{
public:
	msg_queue() {}

	~msg_queue() {}

	void push_back(const T& val)
	{
		m_mutex.lock();
		m_list.push_back(val);
		m_mutex.unlock();
		m_cond.notify_one();
	}

	void push_front(const T& val)
	{
		m_mutex.lock();
		m_list.push_front(val);
		m_mutex.unlock();
		m_cond.notify_one();
	}

	void push_broadcast(const T& val)
	{
		m_mutex.lock();
		m_list.push_back(val);
		m_mutex.unlock();
		m_cond.notify_all();
	}

	T wait()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while (m_list.empty())
			m_cond.wait(lock);
		T val = m_list.front();
		m_list.pop_front();
		return val;
	}

private:
	std::condition_variable m_cond;
	std::mutex				m_mutex;
	std::deque<T>			m_list;
};
} // namespace stdx

#endif // __STDX_QUEUE_H