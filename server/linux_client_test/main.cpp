#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "json.hpp"
#include "request_type.h"
#include <unistd.h>
#include <cstdlib>

using json = nlohmann::json;

#define buffersize 1024

int main(){
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    int sock2 = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(2333);
    connect(sock0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    connect(sock1, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    connect(sock2, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    json request;
    std::string final_message;

    final_message = R"()";
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1001,
    "data":{
        "username":"dqwdqd",
        "pwd":"00000000000000000000000000000000",
        "email":"ne#1231.com",
        "birthday":"2001-08-02"
    }
})";
    // register 10000
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1000,
    "data":{
        "pwd":"00000000000000000000000000000000",
        "email":"ne#1231.com"
    }
})";
    // login 10000
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1001,
    "data":{
        "username":"abab",
        "pwd":"00000000000000000000000000000000",
        "email":"2333.com",
        "birthday":"2000-08-02"
    }
})";
    // register 10001
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1000,
    "data":{
        "pwd":"00000000000000000000000000000000",
        "user_id":10001
    }
})";
    // login 10001
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1009,
    "data":{
        "user_id":10000,
        "to_id":10001
    }
})";

    final_message = R"({
    "type":1001,
    "data":{
        "username":"dqwdqd",
        "pwd":"00000000000000000000000000000000",
        "email":"ne#122e12e31.com",
        "birthday":"2001-08-02"
    }
})";
    // register 10002
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1000,
    "data":{
        "pwd":"00000000000000000000000000000000",
        "email":"ne#122e12e31.com"
    }
})";
    // login 10002
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    
    request["group_name"] = "GG1";
    request["user_id"] = 10000;
    request["data"] = request;
    request["type"] = CREATE_GROUP;
    final_message = to_string(request);
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10000 create group 10000

    request["user_id"] = 10001;
    request["group_id"] = 10000;
    request["data"] = request;
    request["type"] = DELETE_GROUP;
    final_message = to_string(request);
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10001 delete group 10000 failed

    request["user_id"] = 10000;
    request["group_id"] = 10000;
    request["data"] = request;
    request["type"] = DELETE_GROUP;
    final_message = to_string(request);
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10001 delete group 10000

    request["group_name"] = "GG0";
    request["user_id"] = 10000;
    request["data"] = request;
    request["type"] = CREATE_GROUP;
    final_message = to_string(request);
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10000 create group 10000

    request["group_name"] = "GG0";
    request["user_id"] = 10001;
    request["data"] = request;
    request["type"] = CREATE_GROUP;
    final_message = to_string(request);
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10000 create group 10001 naming GG0 failed

    request["group_name"] = "GG1";
    request["user_id"] = 10001;
    request["data"] = request;
    request["type"] = CREATE_GROUP;
    final_message = to_string(request);
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10001 create group 10001 naming GG1

    request["group_name"] = "GG2";
    request["user_id"] = 10002;
    request["data"] = request;
    request["type"] = CREATE_GROUP;
    final_message = to_string(request);
    send(sock2, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10002 create group 10002

    getchar();

    close(sock0);
    close(sock1);

    return 0;
}