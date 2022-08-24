#pragma once
#include <set>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include "json.hpp"
#include "db_operations.h"
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

    //Login and register
    void userLogin(int confd,json &request);
    void userLogout(int confd,json &request);
    void userRegister(int confd,json &request);
    void setLogout(int confd);
    std::string setLogin(int confd,ID user_id);

    //Friends
    void searchUser(int confd,json &request);
    void addFriend(int confd,json &request);
    void getFriendRequest(int confd,json &request);
    void acceptFriend(int confd,json &request);
    void deleteFriend(int confd,json &request);
    void getFriends(int confd,json &request);
    void getPrivateHistory(int confd,json &request);
    
    //Groups
    void createGroup(int confd,json &request);
    void searchGroup(int confd,json &request);
    void addGroup(int confd,json &request);
    void getMemberRequest(int confd,json &request);
    void acceptMember(int confd,json &request);
    void deleteMember(int confd,json &request);
    void inviteMember(int confd,json &request);
    void deleteGroup(int confd,json &request);
    void getGroupMembers(int confd,json &request);
    void getGroupHistory(int confd,json &request);
    void getGroups(int confd,json &request);

    void sendPrivateUnreadMessage(int confd,ID user_id);
    void sendGroupUnreadMessage(int confd,ID user_id);
    void sendPrivateMessage(int confd,json &request);
    void sendGroupMessage(int confd,json &request);
    
    //file
    
public:
    Server(int port);
    void Start();
};