

#pragma once

#include <string>

#include "DD_config.h"

class DDExport IModule
{
public:
	virtual ~IModule() {}
	/// <summary>
	/// 获取模块名字
	/// </summary>
	/// <returns></returns>
	virtual const std::string& getName() = 0;
	/// <summary>
	/// 初始化，加载模块时调用
	/// </summary>
	/// <returns></returns>
	virtual bool Init() = 0;
	/// <summary>
	/// 注销，卸载模块时调用
	/// </summary>
	virtual void Shutdown() = 0;
};