#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#include "EvHttpSvr.h"
//#include "HttpLogic.h"
#include "CHttpReqDispatch.h"
#include "threadres.h"
#include "CConfig.h"
#include "Util.h"
#include "Logger.h"
#include "StrFunc.h"
#include "Helper.h"
#include "SpiderHttp.h"

using namespace std;

int GAME_ID = 0;
short SOURCE = 0;

std::string GetIpHeader( struct evkeyvalq * evKeyValq, const char *key )
{
	std::string strRet = "";
	const char* szHeader = evhttp_find_header(evKeyValq,key);
	if (szHeader) 
	{
		strRet = szHeader;
		ToLower(strRet);

		if (!strRet.empty())
		{
			std::vector<std::string> vsIpList = Helper::explode(strRet.c_str(),",");

			for (std::vector<std::string>::iterator itTmp = vsIpList.begin(); itTmp != vsIpList.end(); itTmp++)
			{
				strRet = TrimStr(*itTmp);
				if (!strRet.empty() && "unknown" != strRet) break;;
			}
		}
	}
	return strRet;
}

std::string GetRealIp( struct evhttp_request *req )
{
	if (NULL == req) return "";
	struct evkeyvalq * evKeyValq = evhttp_request_get_input_headers(req);
	if (evKeyValq)
	{
		std::string strIpHeader = GetIpHeader(evKeyValq,"X-Forwarded-For");
		if(strIpHeader.empty()) strIpHeader = GetIpHeader(evKeyValq,"HTTP_X_FORWARDED_FOR");
		if(strIpHeader.empty()) strIpHeader = GetIpHeader(evKeyValq,"Proxy-Client-IP");
		if(strIpHeader.empty()) strIpHeader = GetIpHeader(evKeyValq,"WL-Proxy-Client-IP");
		if (!strIpHeader.empty()) return strIpHeader;
	}

	char *client_ip;
	u_short client_port;
	evhttp_connection_get_peer(evhttp_request_get_connection(req), &client_ip, &client_port);
	//strRealIp = client_ip;
	return client_ip;
}
void generic_handler(struct evhttp_request *req, void* agr)
{
	//没做的功能，直接转PHP，让PHP返回吧
	std::string strUri;
	const char *uri = evhttp_request_get_uri(req);	//Get Uri
	if (uri) strUri = uri;
	LOGGER(E_LOG_INFO) << "generic_handler uri is :" << strUri;
	std::string strUrl = StrFormatA("http://127.0.0.1:8011/%s",strUri.c_str());
	//std::string strUrl = StrFormatA("http://116.31.115.98:8011/%s",strUri.c_str());
	//POST Data
	std::string strMsg;
	struct evbuffer *evb_req=evhttp_request_get_input_buffer(req); 
	if(evb_req)
	{
		int ev_len = evbuffer_get_length(evb_req); 
		strMsg.reserve(ev_len + 1);
		strMsg.resize(ev_len);
		evbuffer_remove(evb_req, (void*)strMsg.data(), ev_len); 
	}

	LOGGER(E_LOG_INFO) << "generic_handler post msg is :" << strMsg;

	CSpiderHttp spider;
	// 添加客户端过来的所有头信息
	struct evkeyvalq * evHeaders = evhttp_request_get_input_headers(req);
	for(struct evkeyval *header = evHeaders->tqh_first; header != NULL; header = header->next.tqe_next)
	{
		if (header->key && header->value)
		{
			spider.SetHttpHead(StrFormatA("%s: %s",header->key,header->value));
		}
	}

	spider.Get(strUrl.c_str(),strMsg);

	//返回给客户端
	struct evbuffer *evb = evbuffer_new(); 
	if (evb)
	{
		if (spider.GetBodyLen() > 0)
		{
		//	evbuffer_add(evb, "Error", 5); 

			LOGGER(E_LOG_INFO) << "generic_handler GetBodyLen is :" << spider.GetBodyLen();
			evbuffer_add(evb, spider.GetBodyBuf(), spider.GetBodyLen());
		}

		if(req)
		{
			if (spider.GetHeaderLen() > 0)
			{

				LOGGER(E_LOG_INFO) << "generic_handler GetHeaderLen is :" << spider.GetHeaderLen() << ", Header is: " << spider.GetHeaderBuf();
				vector<std::string> vsHeaders = Helper::explode(spider.GetHeaderBuf(), "\r\n");
				vector<std::string> vsHeader;
				std::string strHeader;
				std::string strValue;
				int nStat = 200;
				std::string strStat = "OK";
				bool bFirst = true;
				for(vector<std::string>::iterator itTmp = vsHeaders.begin(); itTmp != vsHeaders.end(); itTmp++)
				{
					if (bFirst)
					{
						bFirst = false;
						vsHeader = Helper::explode((*itTmp).c_str(), " ");
						if (vsHeader.size() > 1)
						{
							nStat = atoi(vsHeader[1].c_str());
						}
						if (vsHeader.size() > 2)
						{
							strStat = vsHeader[2];
						}
					}
					else
					{
						vsHeader = Helper::explode((*itTmp).c_str(), ":");
						if (2 == vsHeader.size())
						{
							strHeader = TrimStr(vsHeader[0]);
							strValue = TrimStr(vsHeader[1]);
							if(!strHeader.empty() && !strValue.empty())
								evhttp_add_header(req->output_headers, strHeader.c_str(), strValue.c_str());
						}
					}
				}
				evhttp_send_reply(req, nStat, strStat.c_str(), evb); 
			}
			else
			{
				evhttp_add_header(req->output_headers, "Content-Type", "text/plain;charset=utf8");
				evhttp_add_header(req->output_headers, "Access-Control-Allow-Origin", "*");
				evhttp_send_reply(req, HTTP_OK, "OK", evb); 
			}
		}

		evbuffer_free(evb); 
	}
}

void api_handler(struct evhttp_request *req, void* agr)
{
	struct evbuffer *evb = NULL; 
	const char *uri = evhttp_request_get_uri(req);	//Get Uri
	if(uri==NULL) 
	{
		return; 
	}        

/*	char output[2048] = "\0";
	char tmp[1024];

	char *decoded_uri;
	decoded_uri = evhttp_decode_uri(uri);

	//解析URI的参数(即GET方法的参数)
	struct evkeyvalq params;
	evhttp_parse_query(decoded_uri, &params);
	sprintf(tmp, "q=%s\n", evhttp_find_header(&params, "aaa"));
	strcat(output, tmp);
	sprintf(tmp, "s=%s\n", evhttp_find_header(&params, "ccc"));
	strcat(output, tmp);
	evhttp_clear_headers(&params);
	free(decoded_uri);
	
	size_t len; 
*/
	//POST Data
	struct evbuffer *evb_req=evhttp_request_get_input_buffer(req); 
	if(evb_req==NULL)
	{
		return;
	}

	int ev_len = evbuffer_get_length(evb_req); 
	std::string strMsg;
	strMsg.reserve(ev_len + 1);
	strMsg.resize(ev_len);
	evbuffer_remove(evb_req, (void*)strMsg.data(), ev_len); 

	std::string strClientIp = GetRealIp( req );
	std::string result; 
	int nDispathRet = CHttpReqDispatch::getInstance().DispatchReq(strMsg.c_str(), ev_len, uri, strClientIp.c_str(), result);
	if (NULL == req) return;

	evb = evbuffer_new(); 
	if (evb)
	{
		if (nDispathRet <= 0 || result.empty())
		{
			evbuffer_add(evb, "Error", 5); 
		}
		else
		{
			evbuffer_add(evb, result.c_str(), result.size());
		}
		if(req)
		{
			evhttp_add_header(req->output_headers, "Content-Type", "text/plain;charset=utf8");
			evhttp_add_header(req->output_headers, "Access-Control-Allow-Origin", "*");
			evhttp_send_reply(req, HTTP_OK, "OK", evb); 
		}

		evbuffer_free(evb); 
	}
}

void show_help() {
	const char *help = "Htt Server\n\n"
		"-d           run as a deamon\n"
		//"-t			  thread number for a http request, default is 10\n"
		"-b			  backlog number for a http request, default is 1024\n"
		"-h           print this help and exit\n"
		"\n";
	fprintf(stderr, "%s", help);
}

int main(int argc, char *argv[])
{
	//默认参数
	int httpd_option_daemon = 0;
	int httpd_option_backlog = 1024;

	//获取参数
	int c;
	while ((c = getopt(argc, argv, "d:b:h")) != -1) {
		switch (c) {
		//case 'p' :
		//	httpd_option_port = atoi(optarg);
		//	break;
		case 'd' :
			httpd_option_daemon = 1;
			break;
		//case 't' :
		//	httpd_option_thread = atoi(optarg);
		//	break;
		case 'b' :
			httpd_option_backlog = atoi(optarg);
			break;
		case 'h' :
		default :
			show_help();
			exit(EXIT_SUCCESS);
		}
	}

	//判断是否设置了-d，以daemon运行
	if (httpd_option_daemon) {
		pid_t pid;
		pid = fork();
		if (pid < 0) 
		{
			perror("fork failed");
			exit(EXIT_FAILURE);
		}
		
		if (pid > 0) 
		{
			//生成子进程成功，退出父进程
			exit(EXIT_SUCCESS);
		}
	}
	
	CConfig& conf = CConfig::getInstance();
	conf.LoadConf((Util::getModulePath()+"/../conf/ucenter.json").c_str());
    
    CLogger::InitLogger(
                        conf.m_loglevel,
                        "HttpServer",
                        conf.server_id,
                        conf.m_logRemoteIp,
                        conf.m_logRemotePort
                        );
	
	CHttpReqDispatch::getInstance().register_handler();
	
	std::string strUcenterUrls[] = {"/api.php", "/api/login.php", "/api/register.php", "/api/bank.php", "/api/clientbase.php", "/api/feedback.php", "/api/task.php",""};
	for (int i = 0; !strUcenterUrls[i].empty(); i++)
	{
		CEvHttpSvr::getInstance().set_cb(strUcenterUrls[i].c_str(), api_handler, NULL);
	}

	int Mytime = time(NULL);
	LOGGER(E_LOG_INFO) << "Mytime:"<<Mytime;

	CEvHttpSvr::getInstance().start(conf.server_ip, conf.server_port, conf.threadNum, httpd_option_backlog,60,generic_handler);//generic_handler
	//CEvHttpSvr::getInstance().start(conf.server_ip, conf.server_port, conf.threadNum, httpd_option_backlog,60,api_handler);
	CLogger::Exit();
	usleep(100);
	return 0;
}

