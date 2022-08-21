#pragma once
#include<set>
#include<sys/socket.h>
#include<sys/types.h>
#include<string>
#include<json.hpp>
#include<db_operations.h>
#include<db_operations.cpp>
using json = nlohmann::json;

class Server{
private:
    LiteChatDatabaseAccess *db;
    int lisfd;
    std::set<int>clientfd;
    fd_set rset,allset;
    int maxfd;
    double last_request[1024];

    void Error(const char *msg,int confd);
    void Accept();
    void Receive();
    void Analyze(int confd,json buf);
    int sendjson(int confd,json result);

    void userLogin(int confd,json &request);
    void userRegister(int confd,json &request);
    void setLogout(int confd);

    void sendUnreadMessage(int confd,int32_t user_id);
    void sendPrivateMessage(json &request);
    void sendGroupMessage(json &request);
    void getFriends(int confd,json &request);
public:
    Server(int port);
    void Start();
};