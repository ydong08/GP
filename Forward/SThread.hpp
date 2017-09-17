//
//  SThread.hpp
//  websocket_forward_prj
//
//  Created by 水深 on 16/11/15.
//  Copyright © 2016年 水深. All rights reserved.
//

#ifndef SThread_hpp
#define SThread_hpp
#include <thread>


/**
 * 线程类
 */
class SThread
{
public:
    // 构造函数
    SThread();

    // 线程函数
    void threadFunction();
    
protected:
    // 线程开始
    virtual void onEventThreadStart() { };
    virtual bool onEventThreadRun() { return true; };
    virtual bool onEventThreadConclude() { return true; };
    
public:
    void stopThread() { _bRun = false; };

	// 开启线程
	bool startThread();
    
private:

    // 用来控制线程是否继续执行
    bool _bRun = false;
	SockPort _sockport;
};

#endif /* SThread_hpp */
