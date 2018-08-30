#include "Http_Server.h"
#include <unordered_map>
#include <iostream>
#include <string>
#include <CFileIO.h>
#include "MJson.h"
#include <MSocket.h>
#include <Public.h>
#include <mutex>
#include "Functions.h"
#include <MyEvent.h>

using namespace std;

extern std::mutex mtx_message;
extern unordered_map<string, ID_List> message;
extern CFileIO File;
extern MSocket sock;
#define BUFFER_SIZE 1024*1024



bool startHttpServer(const char *ip, int port, void(*cb)(struct evhttp_request *, void *), void *arg)
{
    //创建event_base和evhttp
    event_base* base = event_base_new();
    evhttp* http_server = evhttp_new(base);

    if (!http_server)
    {
        return false;
    }
    //绑定到指定地址上
    int ret = evhttp_bind_socket(http_server, ip, port & 0xFFFF);
    if (ret != 0)
    {
        return false;
    }
    //设置事件处理函数
    evhttp_set_gencb(http_server, cb, arg);
    //启动事件循环，当有http请求的时候会调用指定的回调
    cout << "Http Server Running..." << endl;
    event_base_dispatch(base);
    evhttp_free(http_server);
    return true;
}


void MyHttpServerHandler(struct evhttp_request* req, void* arg)
{
    struct evkeyvalq *Req_Header;
    Req_Header = evhttp_request_get_input_headers(req);  //request头
    evbuffer* buf = evbuffer_new();

    const char* uri = (char*)evhttp_request_get_uri(req);   //请求的uri

    if (_stricmp(uri, "/api") == 0 || _stricmp(uri, "/api/") == 0)
    {
        size_t ev_input_data_length = evbuffer_get_length(req->input_buffer);
        unsigned char* ev_input_data = evbuffer_pullup(req->input_buffer, ev_input_data_length);
        string type;
        for (evkeyval* header = Req_Header->tqh_first; header; header = header->next.tqe_next)
        {
            if (_stricmp(header->key, "content-type") == 0)
            {
                type = header->value;
                break;
            }
        }
        if (_stricmp(type.c_str(), "application/json; charset=UTF-8") == 0)  //post
        {
            char input_data[BUF_SIZE];
            memset(input_data, 0, BUF_SIZE);
            memcpy(input_data, ev_input_data, ev_input_data_length);
            Document document;
            document.Parse(input_data);
            if (document.IsObject())
            {
                if (document.HasMember("post_type"))
                {
                    Value &post_type = document["post_type"];

                    if (_stricmp(post_type.GetString(), "message") == 0)
                    {
                        Value &message_type = document["message_type"];
                        if (_stricmp(message_type.GetString(), "group") == 0)
                        {
                            Value &vmessage = document["message"];
                            Value &vgroup_id = document["group_id"];
                            Value &vuser_id = document["user_id"];

                            unordered_map<string, ID_List>::iterator it;
                            string strmsg = vmessage.GetString();
                            Mtx_Lock(mtx_message);
                            it = message.find(strmsg);   //复读口球
                            if (it != message.end())
                            {

                                it->second.count++;
                                it->second.uID_Array[it->second.count] = vuser_id.GetUint64();
                                if (it->second.skip==true&&it->second.count>=Random(3,8))
                                {
                                    it->second.skip = false;
                                }

                            }
                            else
                            {
                                ID_List List;
                                List.count = 0;
                                List.uID_Array[List.count] = vuser_id.GetUint64();
                                message.insert(pair<string, ID_List>(strmsg, List));
                            }
                            Mtx_Unlock(mtx_message);
                            if (_stricmp(vmessage.GetString(), "list") == 0)
                            {
                                Mtx_Lock(mtx_message);
                                for (it = message.begin(); it != message.end(); it++)
                                {
                                    cout << "message: " << it->first << endl;
                                    for (int i = 0; i <= it->second.count; i++)
                                    {
                                        cout << "id: " << it->second.uID_Array[i] << endl;
                                    }

                                }
                                Mtx_Unlock(mtx_message);
                            }
                        }
                        else  //非群消息
                        {

                        }

                    }
                    else  //非消息
                    {

                    }
                }
                else  //不是上报数据
                {

                }
            }
            else  //错误的json包
            {

            }
        }
        evhttp_send_reply(req, HTTP_OK, "OK", buf);
        evbuffer_free(buf);
    }
}

