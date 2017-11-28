

#pragma once



class IModule;
class OutputPacket;

class IKernel
{
public:
	virtual ~IKernel() {}
	/// <summary>
	/// 注册模块
	/// </summary>
	/// <param name="module"></param>
	/// <returns></returns>
	virtual bool installModule(IModule* module) = 0;
	/// <summary>
	/// 卸载模块
	/// </summary>
	/// <param name="module"></param>
	virtual void uninstallModule(IModule* module) = 0;
	/// <summary>
	/// 获取服务器id
	/// </summary>
	/// <returns></returns>
	virtual int getServerId() const = 0;
	/// <summary>
	/// 发送消息到moneyserver
	/// </summary>
	/// <param name="msg"></param>
	/// <returns></returns>
	virtual bool sendMsgToMoney(OutputPacket* msg) = 0;
	/// <summary>
	/// 发送消息到mysql
	/// </summary>
	/// <param name="msg"></param>
	/// <returns></returns>
	virtual bool sendMsgToMySql(OutputPacket* msg) = 0;
	/// <summary>
	/// 发送消息到round
	/// </summary>
	/// <param name="msg"></param>
	/// <returns></returns>
	virtual bool sendMsgToRound(OutputPacket* msg) = 0;
	/// <summary>
	/// 发送消息到trumpt
	/// </summary>
	/// <param name="msg"></param>
	/// <returns></returns>
	virtual bool sendMsgToTrumpt(OutputPacket* msg) = 0;
	/// <summary>
	/// 发送消息到user
	/// </summary>
	/// <param name="msg"></param>
	/// <returns></returns>
	virtual bool sendMsgToUser(OutputPacket* msg) = 0;
	/// <summary>
	/// 发送消息到大厅
	/// </summary>
	/// <param name="hallid"></param>
	/// <param name="msg"></param>
	/// <returns></returns>
	virtual bool sendMsgToHall(int hallid, OutputPacket* msg) = 0;
};