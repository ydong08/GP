#ifndef _ErrorMsg_
#define _ErrorMsg_

class ErrorMsg
{
	public:
		static ErrorMsg* getInstance();
		const char* getErrMsg(int code);
	private:
		ErrorMsg();
		virtual ~ErrorMsg();
	public:
		void init(const char* file = "../conf/errmsg.ini");
		void initFromBuf(const char *pErrMsg);

	private:
		bool binit;
};

#define _EMSG_(code) ErrorMsg::getInstance()->getErrMsg(code)


#endif

