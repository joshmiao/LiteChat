#pragma once
#include<set>
#include<sys/socket.h>
#include<sys/types.h>
#include<string>
#include<json.hpp>
#include<db_operations.h>
#include<db_operations.cpp>
using json = nlohmann::json;

typedef ID int32_t;

class Server{
private:
    LiteChatDatabaseAccess *db;
    int lisfd;
    std::set<int>clientfd;
    fd_set rset,allset;
    int maxfd;
    double last_request[1024];

    void Accept();
    void Receive();
    void Analyze(int confd,json &request);
    int sendjson(int confd,json &result);
    void Error(const char *msg,int confd,int type);

    void userLogin(int confd,json &request);
    void userRegister(int confd,json &request);
    void setLogout(int confd);
    void setLogin(int confd,ID user_id);

    void sendPrivateUnreadMessage(int confd,ID user_id);
    void sendGroupUnreadMessage(int confd,ID user_id);
    void sendPrivateMessage(int confd,json &request);
    void sendGroupMessage(int confd,json &request);
    void getFriends(int confd,json &request);
    void getGroups(int confd,json &request);
    void getPrivateHistory(int confd,json &request);
    void getGroupHistory(int confd,json &request);
public:
    Server(int port);
    void Start();
};