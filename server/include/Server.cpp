#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<random>

#include<set>
#include<vector>
#include<string>
#include<iostream>

#include "Server.h"
#include "json.hpp"
#include<request_type.h>
using json = nlohmann::json;

#define WAIT_TIME 5//0.005
#define TOKEN_LENGTH 32

int Server::sendjson(int confd,json &result){
    std::string str = to_string(result);
    int ret=send(confd,str.c_str(),str.size(),0);
    if(ret<0){
        printf("send to %d failed\n",confd);
        return -1;
    }
    return 0;
}

void Server::Error(const char *msg,int confd=-1,int type=ERROR)
{
    std::string response=(std::string)(msg);
    if(confd>=0)
    {
        json result;
        result["type"]=type;
        json data;
        data["result"]="failed";
        data["error"]=response;
        result["data"]=data;
        sendjson(confd,result);
    }
    else if(confd==-1)exit(1);
}

Server::Server(int port)
{
    //create lisfd
    lisfd=socket(AF_INET,SOCK_STREAM,0);
    if(lisfd==-1)
        Error("socket error");

    struct sockaddr_in myaddr;
    memset(&myaddr,0,sizeof(myaddr));
    myaddr.sin_family=AF_INET;
    myaddr.sin_port=htons(port);
    myaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    int on=1;//reuse addr  
    if((setsockopt(lisfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
        Error("setsockopt error");

    if(bind(lisfd,(struct sockaddr*)&myaddr,sizeof(myaddr))==-1)
        Error("bind error");

    if(listen(lisfd,100)==-1)
        Error("listen error");

    //link database
    //db=new LiteChatDatabaseAccess("mysqlx://LiteChat:Z0136z0136@127.0.0.1");
    db=new LiteChatDatabaseAccess("mysqlx://root:Sail2Boat3A@127.0.0.1");
}

//accept client
void Server::Accept()
{
    int confd=accept(lisfd,NULL,NULL);
    if(confd==-1)
        Error("accept error",-2);
    printf("accept %d\n",confd);
    FD_SET(confd,&allset);
    if(maxfd<confd)maxfd=confd;
    clientfd.insert(confd);
}

//process all client requests
void Server::Receive()
{
    std::vector<int>del;
    for(auto confd:clientfd)
    {
        if(FD_ISSET(confd,&rset))
        {
            char buf[2048]={0};
            int ret=recv(confd,buf,sizeof(char)*2048,0);
            if(ret==-1)
                Error("receive error",confd,RECEIVE);
            else if(ret==0)
            {
                printf("client %d close\n",confd);
                FD_CLR(confd,&allset);
                del.push_back(confd);
            }
            else
            {
                if(json::accept(buf)&&buf[0]=='{'&&buf[strlen(buf)-1]=='}')
                {
                    std::cout<<"accepted a json from "<<confd<<'\n';
                    json request=json::parse(buf);
                    Analyze(confd,request);
                }
                else Error("request format error",confd,REQUEST);
            }
        }
    }

    //check online
    double nowtime=(double)clock()/CLOCKS_PER_SEC;
    for(auto confd:clientfd)
    {
        //std::string str=std::to_string(last_request[confd]); 
        //send(confd,str.c_str(),str.length(),0);
        if(nowtime-last_request[confd]>=WAIT_TIME&&last_request[confd]>0)
        {
            //send(confd,"timeout",7,0);
            printf("client %d offline\n",confd);
            FD_CLR(confd,&allset);
            del.push_back(confd);
        }
    }

    for(auto confd:del)
    {
        clientfd.erase(confd);
        setLogout(confd);
    }
}

void Server::Start()
{
    srand(time(NULL));
    FD_ZERO(&allset);
    FD_SET(lisfd,&allset);
    maxfd=lisfd;

    int cnt=0;

    while(1)
    {
        rset=allset;
        int nready=select(maxfd+1,&rset,NULL,NULL,NULL);
        if(nready==-1)
            Error("select error");
        if(FD_ISSET(lisfd,&rset))
            Accept();
        Receive();
    }
}

//analyze client request
void Server::Analyze(int confd,json &request)
{
    //update last request time
    last_request[confd]=(double)clock()/CLOCKS_PER_SEC;
    if(request["type"]==ALIVE)
    return;
    
    std::cout<<std::setw(4)<<request<<'\n';

    //unfinished
    // std::string token=db->getToken(request["user_id"]);
    // if(token!=(std::string)request["token"])
    // {
    //     Error("token error",confd,TOKEN);
    //     return;
    // }

    int type=(int)request["type"];
    request=request["data"];
    switch(type)
    {
        case LOGIN:userLogin(confd,request);break;
        case REGISTER:userRegister(confd,request);break;
        case PRIVATE_MESSAGE:sendPrivateMessage(confd,request);break;
        case GROUP_MESSAGE:sendGroupMessage(confd,request);break;
        case GET_FRIENDS:getFriends(confd,request);break;
        case GET_GROUPS:getGroups(confd,request);break;
        case GET_HISTORY_PRIVATE:getPrivateHistory(confd,request);break;
        case GET_HISTORY_GROUP:getGroupHistory(confd,request);break;
        case SEARCH_USER:searchUser(confd,request);break;
        case ADD_FRIEND:addFriend(confd,request);break;
        case GET_FRIEND_REQUEST:getFriendRequest(confd,request);break;
        case ACCEPT_FRIEND:acceptFriend(confd,request);break;
        case DELETE_FRIEND:deleteFriend(confd,request);break;
        default :Error("request type error",confd);break;  
    }
}

//Login with user_id or email and password
void Server::userLogin(int confd,json &request)
{
    if(request["user_id"]==request["null"])
        request["user_id"]=0;
    if(request["email"]==request["null"])
        request["email"]="";

    int res=db->userLogin(request["user_id"],request["email"],request["pwd"]);
    if(res==-3)
    {
        std::cout<<confd<<" login failed\n\n";
        Error("user_id and email are both empty",confd,LOGIN);
    }
    else if(res==-2)
    {
        std::cout<<confd<<" login faild\n\n";
        Error("user_id not exist",confd,LOGIN);
    }
    else if(res==-1)
    {
        std::cout<<confd<<" login failed\n\n";
        Error("wrong password",confd,LOGIN);
    }
    else if(res>=USER_ID_BEGIN)
    {
        json result;
        result["type"]=LOGIN;
        json data;
        data["result"]="success_login";

        auto row=db->getBasicUserDataByID(res);
        data["user_id"]=res;
        data["name"]=row.get(1);
        data["email"]=row.get(2);
        data["birthday"]=row.get(3);
        data["avatar_filename"]=row.get(4);
        data["signature"]=row.get(5);
        data["token"]=setLogin(confd,res);
        result["data"]=data;
        sendjson(confd,result);
        std::cout<<confd<<" login success\n\n";
    }
    else{
        std::cout<<confd<<" login failed\n\n";
        Error("unknown error",confd,LOGIN);
    }
}

void Server::userRegister(int confd,json &request)
{
    int res=db->userRegister(request["username"],request["pwd"],request["email"],request["birthday"]);
    if(res==-1)
    {
        std::cout<<confd<<" register failed\n\n";
        Error("email already exist",confd,REGISTER);
    }
    else {
        std::cout<<confd<<" success_register\n\n";
        json result;
        result["type"]=REGISTER;
        json data;
        data["result"]="success_register";
        data["user_id"]=(ID)res;
        result["data"]=data;
        sendjson(confd,result);
    }
}

//after login will request right now
void Server::getFriends(int confd,json &request)
{
    if(request["user_id"]==request["null"])
    {
        std::cout<<confd<<" get friends failed\n\n";
        Error("empty user_id",confd,GET_FRIENDS);
        return;
    }
    ID user_id=request["user_id"];
    //unfinished
    sendPrivateUnreadMessage(confd,request["user_id"]);
}

void Server::getGroups(int confd,json &request)
{
    if(request["user_id"]==request["null"])
    {
        std::cout<<confd<<" get groups failed\n\n";
        Error("empty user_id",confd,GET_GROUPS);
        return;
    }
    ID user_id=request["user_id"];
    //unfinished
    sendGroupUnreadMessage(confd,request["user_id"]);
}

void Server::sendPrivateMessage(int confd,json &request)
{
    ID user_id=request["user_id"];
    ID to_id=request["to_id"];
    std::string content=request["content"];
    std::string time=request["time"];
    db->addUserHistory(time,user_id,to_id,content);
    auto statu=db->getUserStatus(to_id);
    bool to_online=bool(statu.get(1));
    int to_fd=statu.get(2),success=-1;
    if(to_online==true)
    {
        json result;
        result["type"]=PRIVATE_MESSAGE;
        json data;
        data["from_id"]=user_id;
        data["to_id"]=to_id;
        data["content"]=content;
        data["time"]=time;
        result["data"]=data;
        success=sendjson(to_fd,result);
    }
    if(to_online==false||success==-1)
    {
        json result;
        result["type"]=PRIVATE_MESSAGE;
        json data;
        data["result"]="wait_send";
        result["data"]=data;
        sendjson(confd,result);
        db->addUserUnsendMessage(time,to_id,user_id,content);
    }
    else {
        json result;
        result["type"]=PRIVATE_MESSAGE;
        json data;
        data["result"]="success_send";
        result["data"]=data;
        sendjson(confd,result);
        std::cout<<"send to "<<to_id<<" private message success\n\n";
    }
}

void Server::sendGroupMessage(int confd,json &request)
{
    ID user_id=request["user_id"];
    ID group_id=request["group_id"];
    std::string content=request["content"];
    std::string time=request["time"];

    json message;
    message["type"]=GROUP_MESSAGE;
    json data;
    data["from_id"]=user_id;
    data["group_id"]=group_id;
    data["content"]=content;
    data["time"]=time;
    message["data"]=data;
    
    db->addGroupHistory(time,user_id,group_id,content);
    json result;
    result["type"]=GROUP_MESSAGE;
    json rdata;
    rdata["result"]="success_send";
    result["data"]=rdata;
    std::cout<<"send to "<<group_id<<" group message success\n\n";
    sendjson(confd,result);

    auto group_member=db->getGroupMember(group_id);
    while(group_member.count()>0)
    {
        ID to_id=(ID)(group_member.fetchOne().get(0));
        auto statu=db->getUserStatus(to_id);
        if(to_id==user_id)
            continue;
        bool to_online=bool(statu.get(1));
        int to_fd=statu.get(2),success=-1;
        if(to_online==true)
        {
            message["data"]["to_id"]=to_id;
            success=sendjson(to_fd,message);
        }
        if(to_online==false||success==-1)
        {
            db->addGroupUnsendMessage(time,to_id,user_id,group_id,content);
        }
        else {
            std::cout<<"send to "<<to_id<<" group message success\n\n";
        }
    }
}

//send unread message to user after login
void Server::sendPrivateUnreadMessage(int confd,ID user_id)
{ 
    std::vector<json>message_bundle;
    auto private_message=db->searchUserUnsendMessage(user_id);   
    while(private_message.count()>0)
    {
        auto row=private_message.fetchOne();
        json message;
        message["type"]=PRIVATE_MESSAGE;
        json data;
        data["from_id"]=row.get(2);
        data["to_id"]=row.get(1);
        data["content"]=row.get(3);
        data["time"]=row.get(0);
        message["data"]=data;
        message_bundle.push_back(message);
    }

    json result;
    result["type"]=UNREAD_PRIVATE_MESSAGE;
    result["data"]=json(message_bundle);
    int success=sendjson(confd,result);
    if(success==0)db->deleteUserUnsendMessage(user_id);
}

void Server::sendGroupUnreadMessage(int confd,ID user_id)
{
    std::vector<json>message_bundle;
    auto group_message=db->searchGroupUnsendMessage(user_id);   
    while(group_message.count()>0)
    {
        auto row=group_message.fetchOne();
        json message;
        message["type"]=GROUP_MESSAGE;
        json data;
        data["group_id"]=row.get(3);
        data["from_id"]=row.get(2);
        data["to_id"]=row.get(1);
        data["time"]=row.get(0);
        data["content"]=row.get(4);
        message["data"]=data;
        message_bundle.push_back(message);
    }

    json result;
    result["type"]=UNREAD_GROUP_MESSAGE;
    result["data"]=json(message_bundle);
    int success=sendjson(confd,result);
    if(success==0)db->deleteGroupUnsendMessage(user_id);
}

//judge if a user login with the fd first
void Server::setLogout(int confd)
{
    db->updateUserStatusWhenLogout(confd);
}

const char charlist[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
std::string Server::setLogin(int confd,ID user_id)
{
    char token[TOKEN_LENGTH+1];
    for(int i=0;i<TOKEN_LENGTH;i++)
    token[i]=charlist[rand()%62];
    token[TOKEN_LENGTH]=0;
    db->updateUserStatusWhenLogin(user_id,confd,std::string(token));
    return std::string(token);
}

void Server::getPrivateHistory(int confd,json &request)
{
    if(request["user_id"]==request["null"]||request["from_id"]==request["null"])
    {
        Error("empty user_id or to_id",confd,GET_HISTORY_PRIVATE);
        return;
    }
    if(request["begin_time"]==request["null"])
        request["begin_time"]="'2000-01-01'";
    else request["begin_time"]="'"+(std::string)request["begin_time"]+"'";
    if(request["end_time"]==request["null"])
        request["end_time"]="now()";
    else request["end_time"]="'"+(std::string)request["end_time"]+"'";

    std::vector<json>res;
    ID user_id=request["user_id"];
    ID from_id=request["from_id"];
    auto all_message=db->searchUserHistory(from_id,user_id,request["begin_time"],request["end_time"]);
    while(all_message.count()>0)
    {
        auto row=all_message.fetchOne();
        json message;
        message["type"]=PRIVATE_MESSAGE;
        json data;
        data["from_id"]=row.get(2);
        data["to_id"]=row.get(1);
        data["time"]=row.get(0);
        data["content"]=row.get(3);
        message["data"]=data;
        res.push_back(message);
    }

    json result;
    result["type"]=GET_HISTORY_PRIVATE; 
    result["data"]=json(res);
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("send failed",confd,GET_HISTORY_PRIVATE);
    }
    else{
        std::cout<<confd<<" get private history successfully\n\n";
    }
}

void Server::getGroupHistory(int confd,json &request)
{
    if(request["group_id"]==request["null"]||request["user_id"]==request["null"])
    {
        Error("empty group_id or user_id",confd,GET_HISTORY_GROUP);
        return;
    }
    if(request["begin_time"]==request["null"])
        request["begin_time"]="'2000-01-01'";
    else request["begin_time"]="'"+(std::string)request["begin_time"]+"'";
    if(request["end_time"]==request["null"])
        request["end_time"]="now()";
    else request["end_time"]="'"+(std::string)request["end_time"]+"'";
    std::vector<json>res;
    ID group_id=request["group_id"];
    ID to_id=request["user_id"];
    auto all_message=db->searchGroupHistory(to_id,group_id,request["begin_time"],request["end_time"]);
    while(all_message.count()>0)
    {
        auto row=all_message.fetchOne();
        json message;
        message["type"]=GROUP_MESSAGE;
        json data;
        data["group_id"]=row.get(3);
        data["from_id"]=row.get(2);
        data["to_id"]=row.get(1);
        data["time"]=row.get(0);
        data["content"]=row.get(4);
        message["data"]=data;
        res.push_back(message);
    }

    json result;
    result["type"]=GET_HISTORY_GROUP;
    result["data"]=json(res);
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("send failed",confd,GET_HISTORY_GROUP);
    }
    else{
        std::cout<<confd<<" get group history successfully\n\n";
    }
}

void Server::searchUser(int confd,json &request)
{
    std::string keyword=(std::string)request["keyword"];
    bool Name=true;
    ID id=0;
    if(keyword.length()<=6)
    for(auto c:keyword)
    if(isdigit(c))
        id=id*10+c-'0';
    else {
        id=0;
        break;
    }

    auto users=db->searchUser(id,keyword);
    std::vector<json>res;
    while(users.count()>0)
    {
        auto row=users.fetchOne();
        json user;
        user["user_id"]=row.get(0);
        user["name"]=row.get(1);
        user["email"]=row.get(2);
        user["birthday"]=row.get(3);
        user["signature"]=row.get(5);

        //unfinished
        //user avatar_filename and resoure?
        
        res.push_back(user);
    }
    json result;
    result["type"]=SEARCH_USER;
    result["data"]=json(res);
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("search failed",confd,SEARCH_USER);
    }
    else{
        std::cout<<confd<<" search user successfully\n\n";
    }
}

void Server::addFriend(int confd,json &request)
{
    ID from_id=request["user_id"];
    ID to_id=request["to_id"];
    std::string message=(std::string)request["message"];

    int res=db->createFriendRequest(from_id,to_id,message);
    if(res==-1)
    {
        Error("you are already friends",confd,ADD_FRIEND);
    }    
    else{
        json result;
        result["type"]=ADD_FRIEND;
        json data;
        data["result"]="send successfully";
        result["data"]=data;
        sendjson(confd,result);
    }
}

void Server::getFriendRequest(int confd,json &request)
{
    auto requests=db->getFriendRequest(0,request["user_id"]);
    std::vector<json>res;
    while(requests.count()>0)
    {
        auto row=requests.fetchOne();
        json request;
        request["from_id"]=row.get(0);
        request["message"]=row.get(2);
        res.push_back(request);
    }
    json result;
    result["type"]=GET_FRIEND_REQUEST;
    result["data"]=json(res);
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("get failed",confd,GET_FRIEND_REQUEST);
    }
    else{
        std::cout<<confd<<" get friend request successfully\n\n";
    }
}

void Server::acceptFriend(int confd,json &request)
{
    ID from_id=request["from_id"];
    ID to_id=request["user_id"];
    bool accept=(bool)request["accept"];
    db->deleteFriendRequest(from_id,to_id);

    if(!accept)
    {
        json result;
        result["type"]=ACCEPT_FRIEND;
        json data;
        data["result"]="refuse successfully";
        result["data"]=data;
        sendjson(confd,result);
        return;
    }

    db->createFriendRelation(from_id,to_id);
    json result;
    result["type"]=ACCEPT_FRIEND;
    json data;
    data["result"]="accept successfully";
    result["data"]=data;
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("accept failed",confd,ACCEPT_FRIEND);
    }
    else{
        std::cout<<confd<<" accept friend successfully\n\n";
    }
}

void Server::deleteFriend(int confd,json &request)
{
    ID id1=request["friend_id"];
    ID id2=request["user_id"];
    db->deleteFriendRelation(id1,id2);
    json result;
    result["type"]=DELETE_FRIEND;
    json data;
    data["result"]="delete successfully";
    result["data"]=data;
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("delete failed",confd,DELETE_FRIEND);
    }
    else{
        std::cout<<confd<<" delete friend successfully\n\n";
    }
}