
#include <MSocket.h>
#include <iostream>
#include "Http_Server.h"
#include <random>
#include <unordered_map>
#include <mutex>
#include <CFileIO.h>
#include <MyEvent.h>
#include <Public.h>
#include <assert.h>
using namespace std;

#define  GROUP_ID 870187255
//#define  GROUP_ID 454237660

extern std::mutex mtx_message;
extern unordered_map<string, ID_List> message;
extern CFileIO File;
extern MSocket sock;



int Random(int min, int max)
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<> dis(min, max);
    auto dice = std::bind(dis, engine);
    return dice();
}

void Ban(uint64_t group_id, uint64_t user_id)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sock.Connect(s, "127.0.0.1", 5700);
    char buff[1024];
    memset(buff, 0, 1024);
    int duration = Random(1, 100);
    if (duration >= 1 && duration <= 70)
    {
        duration = 60;
    }
    else if (duration >= 71 && duration <= 90)
    {
        duration = 60 * 2;
    }
    else if (duration >= 91 && duration <= 99)
    {
        duration = 60 * 3;
    }
    else
    {
        duration = 60 * 5;
    }
    string cmd = GET_START;
    cmd += R"(/set_group_ban?)";
    cmd += "group_id=" + std::to_string(group_id);
    cmd += "&user_id=" + std::to_string(user_id);
    cmd += "&duration=" + std::to_string(duration);
    cmd += GET_END;
    sock.Send(s, cmd.c_str(), cmd.size());

    sock.Close(s);
    cout << "banned:" << user_id << " duration: " << duration << endl;

}



void SendGroupMsg(uint64_t group_id, string Msg)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sock.Connect(s, "127.0.0.1", 5700);
    string cmd = GET_START;
    cmd += R"(/send_group_msg?)";
    cmd += "group_id=" + std::to_string(group_id);
    cmd += "&message=" + Msg;
    cmd += GET_END;
    cout << cmd << endl;
    sock.Send(s, cmd.c_str(), cmd.size());

    //char buff[1024];
    //memset(buff, 0, 1024);
    //sock.Recv(s, buff, 1024);
    //cout << buff << endl;
    sock.Close(s);
};


void RepeatBan()
{
    while (true)
    {
        unordered_map<string, ID_List>::iterator it;
        Mtx_Lock(mtx_message);
        for (it = message.begin(); it != message.end();)
        {
            if (it->second.skip == false)
            {
                int ban_id_count = Random(1, it->second.count);
                Ban(GROUP_ID, it->second.uID_Array[ban_id_count]);
                SendGroupMsg(GROUP_ID, "让我康康沙雕群友又在复读什么鸡*");
                message.erase(it++);
            }
            else
            {
                it++;
            }
        }
        Mtx_Unlock(mtx_message);
        MSleep(1, "ms");
    }

}

void CleanMemory()
{
    while (true)
    {
        MSleep(5 * 60, "s");   //五分钟清理一次历史消息
        unordered_map<string, ID_List>::iterator it;
        Mtx_Lock(mtx_message);
        for (it = message.begin(); it != message.end();)
        {
            message.erase(it++);
        }
        cout << "memory cleaned" << endl;
        Mtx_Unlock(mtx_message);
    }
}