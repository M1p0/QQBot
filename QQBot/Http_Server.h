#pragma once

#include <iostream>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
#include <evhttp.h>

bool startHttpServer(const char *ip, int port, void(*cb)(struct evhttp_request *, void *), void *arg);
void MyHttpServerHandler(struct evhttp_request* req, void* arg);

#define GET_START "GET "
#define GET_END " HTTP/1.1\r\n\r\n"
#define POST_START  "POST "
#define POST_END " HTTP/1.1\r\n\r\n"


struct ID_List
{
    uint64_t uID_Array[20] = { 0 };
    int count = 0;
    bool skip = true;
};

