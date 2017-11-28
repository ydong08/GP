//
//  SThread.cpp
//  websocket_forward_prj
//
//  Created by 水深 on 16/11/15.
//  Copyright © 2016年 水深. All rights reserved.
//

#include "SThread.hpp"
#include <functional>

SThread::SThread()
{
    
}

/**
 * 开始线程
 */
bool SThread::startThread()
{
    // 如果正在运行 直接返回
    if (_bRun)
    {
        return false;
    }
    
    std::function<void()> fp;
    std::thread _pthread(&SThread::threadFunction, this);
    _pthread.detach();
    return true;
}

/**
 * 线程函数
 */
void SThread::threadFunction()
{
    onEventThreadStart();
    
    // 设置线程正在运行
    _bRun = true;
    
    while (_bRun) {
        
        if (false == onEventThreadRun())
        {
            break;
        }
        
    }
    
    onEventThreadConclude();
    _bRun = false;
    
    return ;
}