#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#include<set>
#include<vector>
#include<string>
#include<iostream>

#include<Server.h>
#include<json.hpp>
#include<request_type.h>
using json = nlohmann::json;

#define WAIT_TIME 0.005

int Server::sendjson(int confd,json result){
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
        result["result"]="failed";
        result["error"]=response;
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
    db=new LiteChatDatabaseAccess("mysqlx://LiteChat:Z0136z0136@127.0.0.1");
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
                    Analyze(confd,json::parse(buf));
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
void Server::Analyze(int confd,json request)
{
    //update last request time
    last_request[confd]=(double)clock()/CLOCKS_PER_SEC;
    if(request["type"]==ALIVE)
    return;
    //std::cout<<std::setw(4)<<request<<'\n';
    if(request["type"]==LOGIN)
    {
        userLogin(confd,request);
    }
    else if(request["type"]==REGISTER)
    {
        userRegister(confd,request);
    }
    else if(request["type"]==PRIVATE_MESSAGE)
    {
        sendPrivateMessage(confd,request);
    }
    else if(request["type"]==GROUP_MESSAGE)
    {
        sendPrivateMessage(confd,request);
    }
    else if(request["type"]==GET_FREIENDS)
    {
        getFriends(confd,request);
    }
    else if(request["type"]==GET_HISTORY_PRIVATE)
    {
        getPrivateHistory(confd,request);
    }
    else if(request["type"]==GET_HISTORY_GROUP)
    {
        getGroupHistory(confd,request);
    }
    else Error("request type error",confd);
}

//Login with user_id or email and password
void Server::userLogin(int confd,json &request)
{
    if(request["user_id"]==request["null"])
        request["user_id"]=0;
    if(request["email"]==request["null"])
        request["email"]="";
    if(request["pwd"]==request["null"])
        request["pwd"]="";
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
        std::cout<<confd<<" login success\n\n";
        json result;
        result["type"]=LOGIN;
        result["result"]="success_login";
        result["user_id"]=res;
        //unfinished
        sendjson(confd,result);
        setLogin(confd,res);
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
        result["result"]="success_register";
        result["user_id"]=(ID)res;
        sendjson(confd,result);
    }
}

//after login will request right now
void Server::getFriends(int confd,json &request)
{
    if(request["user_id"]==request["null"])
    {
        std::cout<<confd<<" get friends failed\n\n";
        Error("empty user_id",confd,GET_FREIENDS);
        return;
    }
    ID user_id=request["user_id"];
    //unfinished
    sendUnreadMessage(confd,request["user_id"]);
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
        json message;
        message["type"]=PRIVATE_MESSAGE;
        message["from_id"]=user_id;
        message["to_id"]=to_id;
        message["content"]=content;
        message["time"]=time;
        success=sendjson(to_fd,message);
    }
    if(to_online==false||success==-1)
    {
        json result;
        result["type"]=PRIVATE_MESSAGE;
        result["result"]="wait_send";
        sendjson(confd,result);
        db->addUnsendMessageFromUser(time,to_id,user_id,content);
    }
    else {
        json result;
        result["type"]=PRIVATE_MESSAGE;
        result["result"]="success_send";
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
    message["from_id"]=user_id;
    message["group_id"]=group_id;
    message["content"]=content;
    message["time"]=time;
    
    db->addGroupHistory(time,user_id,group_id,content);
    json result;
    result["type"]=GROUP_MESSAGE;
    result["result"]="success_send";
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
            message["to_id"]=to_id;
            success=sendjson(to_fd,message);
        }
        if(to_online==false||success==-1)
        {
            db->addUnsendMessageFromGroup(time,to_id,user_id,group_id,content);
        }
        else {
            std::cout<<"send to "<<to_id<<" group message success\n\n";
        }
    }
}

//send unread message to user after login
void Server::sendUnreadMessage(int confd,ID user_id)
{ 
    std::vector<json>message_bundle;
    auto private_message=db->searchUnsendMessageFromUser(user_id);   
    while(private_message.count()>0)
    {
        auto row=private_message.fetchOne();
        json message;
        message["type"]=PRIVATE_MESSAGE;
        message["from_id"]=row.get(2);
        message["to_id"]=row.get(1);
        message["time"]=row.get(0);
        message["content"]=row.get(3);
        message_bundle.push_back(message);
    }
    json result;
    result["type"]=UNREAD_PRIVATE_MESSAGE;
    result["contents"]=json(message_bundle);
    int success=sendjson(confd,result);
    if(success==0)db->deleteUnsendMessageFromUser(user_id);

    message_bundle.clear();
    auto group_message=db->searchUnsendMessageFromGroup(user_id);   
    while(private_message.count()>0)
    {
        auto row=private_message.fetchOne();
        json message;
        message["type"]=GROUP_MESSAGE;
        message["group_id"]=row.get(3);
        message["from_id"]=row.get(2);
        message["to_id"]=row.get(1);
        message["time"]=row.get(0);
        message["content"]=row.get(4);
        message_bundle.push_back(message);
    }

    result["type"]=UNREAD_GROUP_MESSAGE;
    result["contents"]=json(message_bundle);
    success=sendjson(confd,result);
    if(success==0)db->deleteUnsendMessageFromGroup(user_id);
}

//judge if a user login with the fd first
void Server::setLogout(int confd)
{
    //unfinished
}

void Server::setLogin(int confd,ID user_id)
{
    //unfinished    
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
        message["from_id"]=row.get(2);
        message["to_id"]=row.get(1);
        message["time"]=row.get(0);
        message["content"]=row.get(3);
        res.push_back(message);
    }

    json result;
    result["type"]=GET_HISTORY_PRIVATE; 
    result["contents"]=json(res);
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
        message["group_id"]=row.get(3);
        message["from_id"]=row.get(2);
        message["to_id"]=row.get(1);
        message["time"]=row.get(0);
        message["content"]=row.get(4);
        res.push_back(message);
    }

    json result;
    result["type"]=GET_HISTORY_GROUP;
    result["contents"]=json(res);
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("send failed",confd,GET_HISTORY_GROUP);
    }
    else{
        std::cout<<confd<<" get group history successfully\n\n";
    }
}