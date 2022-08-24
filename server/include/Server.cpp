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
#include "db_operations.h"
#include "db_table_column.h"
#include "request_type.h"
using json = nlohmann::json;

#define WAIT_TIME 500//0.005
#define TOKEN_LENGTH 32

int Server::sendjson(int confd,json &result){
    std::string str = to_string(result);
    std::cout << std::setw(4) << result << std::endl;
    try{
        int ret=send(confd,str.c_str(),str.size(),0);
        if(ret<0){
            printf("send to %d failed\n",confd);
            return -1;
        }
    }
    catch(...){
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
    else if(confd==-1)
    {
        std::cout<<response<<'\n';
        exit(1);
    }
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
    //db=new LiteChatDatabaseAccess("mysqlx://root:Sail2Boat3A@127.0.0.1");
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
            if(ret<=0)
            {
                printf("client %d close\n",confd);
                FD_CLR(confd,&allset);
                del.push_back(confd);
            }
            else
            {
                std::string str(buf);
                int begin=0;
                while(1)
                {
                    if(str[begin]!='{')
                    {
                        Error("request format error",confd,REQUEST);
                        break;
                    }

                    int k=begin+1,seq=1;
                    for(;seq&&k<str.length();k++)
                    if(str[k]=='{')seq++;
                    else if(str[k]=='}')seq--;
                    if(seq)
                    {
                        Error("request format error",confd,REQUEST);
                        break;
                    }

                    std::string req=str.substr(begin,k-begin);
                    if(json::accept(req.c_str())&&req[0]=='{'&&req[req.length()-1]=='}')
                    {
                        std::cout<<"accepted a json from "<<confd<<'\n';
                        json request=json::parse(req.c_str());
                        Analyze(confd,request);
                    }
                    else{
                        Error("request format error",confd,REQUEST);
                        break;
                    }
                    
                    if(k==str.length())break;
                    else begin=k;
                }
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
    
    if(request["token"]!=request["null"])
    {
        std::string token(db->getUserStatus((ID)request["data"]["user_id"]).get(USER_STATUS_TOKEN));
        if(token!=(std::string)request["token"])
        {
            Error("token error",confd,TOKEN);
            return;
        }
    }

    int type=(int)request["type"];
    request=request["data"];
    switch(type)
    {
        case LOGIN:userLogin(confd,request);break;
        case LOGOUT:userLogout(confd,request);break;
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
        case CREATE_GROUP:createGroup(confd,request);break;
        case SEARCH_GROUP:searchGroup(confd,request);break;
        case ADD_GROUP:addGroup(confd,request);break;
        case GET_MEMBER_REQUEST:getMemberRequest(confd,request);break;
        case ACCEPT_MEMBER:acceptMember(confd,request);break;
        case DELETE_MEMBER:deleteMember(confd,request);break;
        case INVITE_MEMBER:inviteMember(confd,request);break;
        case DELETE_GROUP:deleteGroup(confd,request);break;
        case GET_GROUP_MEMBERS:getGroupMembers(confd,request);break;
        default :Error("request type error",confd);break;  
    }
}

void Server::userLogout(int confd,json &request)
{
    std::cout<<confd<<" logout successfully\n\n";
    setLogout(confd);
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
        data["result"]="login successfully";

        auto row=db->getBasicUserDataByID(res);
        data["user_id"]=(ID)res;
        data["user_name"]=row.get(BASIC_USER_DATA_USER_NAME);
        data["email"]=row.get(BASIC_USER_DATA_EMAIL);
        data["birthday"]=row.get(BASIC_USER_DATA_BIRTHDAY);
        data["avatar_filename"]=row.get(BASIC_USER_DATA_AVATAR_FILENAME);
        data["signature"]=row.get(BASIC_USER_DATA_SIGNATURE);
        data["token"]=setLogin(confd,res);
        result["data"]=data;
        sendjson(confd,result);
        std::cout<<confd<<" login successfully\n\n";
    }
    else{
        std::cout<<confd<<" login failed\n\n";
        Error("unknown error",confd,LOGIN);
    }
}

void Server::userRegister(int confd,json &request)
{
    if(request["birthday"]==request["null"])
        request["birthday"]="2002-01-01";
    int res=db->userRegister(request["username"],request["pwd"],request["email"],request["birthday"]);
    if(res==-1)
    {
        std::cout<<confd<<" register failed\n\n";
        Error("email already exist",confd,REGISTER);
    }
    else {
        std::cout<<confd<<" register successfully\n\n";
        json result;
        result["type"]=REGISTER;
        json data;
        data["result"]="register successfully";
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
    auto friends=db->getFriendRelation(user_id,0);
    std::vector<json>data;
    while(friends.count()>0)
    {
        json _friend;
        ID friend_id;
        _friend["friend_id"]=friend_id=(ID)friends.fetchOne().get(FRIEND_RELATION_USER2);
        _friend["is_online"]=(int)db->getUserStatus(friend_id).get(USER_STATUS_IS_ONLINE);
        auto row=db->getBasicUserDataByID(friend_id);
        _friend["friend_name"]=row.get(BASIC_USER_DATA_USER_NAME);
        _friend["email"]=row.get(BASIC_USER_DATA_EMAIL);
        _friend["birthday"]=row.get(BASIC_USER_DATA_BIRTHDAY);
        _friend["avatar_filename"]=row.get(BASIC_USER_DATA_AVATAR_FILENAME);
        _friend["signature"]=row.get(BASIC_USER_DATA_SIGNATURE);

        data.push_back(_friend);
    }
    json result;
    result["type"]=GET_FRIENDS;
    result["data"]=data;
    sendjson(confd,result);
    std::cout<<"get friends successfully\n";

    sendPrivateUnreadMessage(confd,request["user_id"]);
}

void Server::sendPrivateMessage(int confd,json &request)
{
    ID user_id=request["user_id"];
    ID to_id=request["to_id"];
    std::string content=request["content"];
    std::string time=request["time"];
    db->addUserHistory(time,user_id,to_id,content);
    auto status=db->getUserStatus(to_id);
    bool to_online=bool(status.get(USER_STATUS_IS_ONLINE));
    int success=-1;
    if(to_online==true)
    {
        int to_fd=(int)status.get(USER_STATUS_HANDLE);

        json result;
        result["type"]=PRIVATE_MESSAGE;
        json data;
        data["from_id"]=(ID)user_id;
        data["to_id"]=(ID)to_id;
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
        data["result"]="send successfully";
        result["data"]=data;
        sendjson(confd,result);
        std::cout<<"send to "<<to_id<<" private message successfully\n\n";
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
    data["from_id"]=(ID)user_id;
    data["group_id"]=(ID)group_id;
    data["content"]=content;
    data["time"]=time;
    message["data"]=data;
    
    db->addGroupHistory(time,user_id,group_id,content);
    json result;
    result["type"]=GROUP_MESSAGE;
    json rdata;
    rdata["result"]="send successfully";
    result["data"]=rdata;
    std::cout<<"send to "<<group_id<<" group message successfully\n\n";
    sendjson(confd,result);

    auto group_member=db->getGroupMember(group_id);
    while(group_member.count()>0)
    {
        ID to_id=(ID)(group_member.fetchOne().get(FRIEND_RELATION_USER2));
        auto statu=db->getUserStatus(to_id);
        if(to_id==user_id)
            continue;
        bool to_online=bool(statu.get(USER_STATUS_IS_ONLINE));
        int success=-1;
        if(to_online==true)
        {
            int to_fd=(int)statu.get(USER_STATUS_HANDLE);
            message["data"]["to_id"]=(ID)to_id;
            success=sendjson(to_fd,message);
        }
        if(to_online==false||success==-1)
        {
            db->addGroupUnsendMessage(time,to_id,user_id,group_id,content);
        }
        else {
            std::cout<<"send to "<<to_id<<" group message successfully\n\n";
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
        std::string time=(std::string)row.get(USER_UNSEND_MESSAGE_SEND_TIME);
        reverse(time.begin(),time.end());
        time=time.substr(3);
        reverse(time.begin(),time.end());
        data["time"]=time;
        data["to_id"]=user_id;
        data["from_id"]=(ID)row.get(USER_UNSEND_MESSAGE_SRC_USER_ID);
        data["content"]=row.get(USER_UNSEND_MESSAGE_CONTENT);
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
        std::string time=(std::string)row.get(GROUP_UNSEND_MESSAGE_SEND_TIME);
        reverse(time.begin(),time.end());
        time=time.substr(3);
        reverse(time.begin(),time.end());
        data["time"]=time;
        data["to_id"]=user_id;
        data["from_id"]=(ID)row.get(GROUP_UNSEND_MESSAGE_SRC_USER_ID);
        data["group_id"]=(ID)row.get(GROUP_UNSEND_MESSAGE_DST_GROUP_ID);
        data["content"]=(std::string)row.get(GROUP_UNSEND_MESSAGE_CONTENT);
        message["data"]=data;
        message_bundle.push_back(message);
    }

    json result;
    result["type"]=UNREAD_GROUP_MESSAGE;
    result["data"]=json(message_bundle);
    int success=sendjson(confd,result);
    if(success==0)db->deleteGroupUnsendMessage(user_id, 0);
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
        Error("empty user_id or from_id",confd,GET_HISTORY_PRIVATE);
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
        std::string time=(std::string)row.get(USER_HISTORY_SEND_TIME);
        reverse(time.begin(),time.end());
        time=time.substr(3);
        reverse(time.begin(),time.end());
        data["time"]=time;
        data["to_id"]=(ID)row.get(USER_HISTORY_DST_USER_ID);
        data["from_id"]=(ID)row.get(USER_HISTORY_SRC_USER_ID);
        data["content"]=row.get(USER_HISTORY_CONTENT);
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
        std::string time=(std::string)row.get(GROUP_HISTORY_SEND_TIME);
        reverse(time.begin(),time.end());
        time=time.substr(3);
        reverse(time.begin(),time.end());
        data["time"]=time;
        //data["to_id"]=(ID)row.get(GROUP_HISTORY_DST_GROUP_ID);
        data["from_id"]=(ID)row.get(GROUP_HISTORY_SRC_USER_ID);
        data["group_id"]=(ID)row.get(GROUP_HISTORY_DST_GROUP_ID);
        data["content"]=row.get(GROUP_HISTORY_CONTENT);
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
        user["user_id"]=(ID)row.get(BASIC_USER_DATA_USER_ID);
        user["user_name"]=row.get(BASIC_USER_DATA_USER_NAME);
        user["email"]=row.get(BASIC_USER_DATA_EMAIL);
        user["birthday"]=row.get(BASIC_USER_DATA_BIRTHDAY);
        user["avatar_filename"]=row.get(BASIC_USER_DATA_AVATAR_FILENAME);
        user["signature"]=row.get(BASIC_USER_DATA_SIGNATURE);
        
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
    if (request["message"] == request["null"])
        request["message"] = (std::string) "My id is "+std::to_string(from_id);
    std::string message=(std::string)request["message"];

    int res=db->createFriendRequest(from_id,to_id, message);
    if(res==-1)
    {
        Error("you have already requested",confd,ADD_FRIEND);
    }
    else if(res==-2)
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
        request["from_id"]=(ID)row.get(FRIEND_REQUEST_USER_FROM);
        request["name"]=db->getBasicUserDataByID((ID)request["from_id"]).get(BASIC_USER_DATA_USER_NAME);
        request["message"]=row.get(FRIEND_REQUEST_REQUEST_MESSAGE);
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

    auto status=db->getUserStatus(from_id);
    bool is_online=(bool)status.get(USER_STATUS_IS_ONLINE);
    if(is_online==true)
    {
        json _friend;
        ID friend_id=to_id;
        _friend["friend_id"]=friend_id;
        _friend["is_online"]=(int)status.get(USER_STATUS_IS_ONLINE);
        auto row=db->getBasicUserDataByID(friend_id);
        _friend["friend_name"]=row.get(BASIC_USER_DATA_USER_NAME);
        _friend["email"]=row.get(BASIC_USER_DATA_EMAIL);
        _friend["birthday"]=row.get(BASIC_USER_DATA_BIRTHDAY);
        _friend["avatar_filename"]=row.get(BASIC_USER_DATA_AVATAR_FILENAME);
        _friend["signature"]=row.get(BASIC_USER_DATA_SIGNATURE);
        std::vector<json>data;
        data.push_back(_friend);
        json result;
        result["type"]=GET_FRIENDS;
        result["data"]=data;
        int fd=(int)status.get(USER_STATUS_HANDLE);
        sendjson(fd,result);
        std::cout<<fd<<" get a new friend\n"; 
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

void Server::inviteMember(int confd,json &request){
    ID user_id = request["user_id"];
    ID to_id = request["to_id"];
    ID group_id = request["group_id"];
    
    if (db->getGroupsOfAUser(to_id, group_id).count() != 0){
        Error("failed: the invitee is already in this group",confd,ADD_GROUP);
        return;
    }
    if (db->getGroupsOfAUser(user_id, group_id).count() == 0){
        Error("failed: you are not in this group",confd,ADD_GROUP);
        return;
    }
    db->addUserToGroup(group_id,to_id);
    
    json result, data;
    result["type"] = INVITE_MEMBER;
    data["result"] = "invited successfully";
    result["data"] = data;
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("Invitation message sending failed",confd,INVITE_MEMBER);
    }
    else{
        std::cout<<confd<<" Invitation message sent successfully\n\n";
    }
}

void Server::deleteGroup(int confd,json &request){
    ID user_id = request["user_id"];
    ID group_id = request["group_id"];
    if(user_id != (ID)db->getBasicGroupData(group_id).get(BASIC_GROUP_DATA_OWNER))
    {
        Error("you are not the owner of this group",confd,DELETE_GROUP);
        return;
    }

    db->deleteGroupUnsendMessage(0, group_id);
    db->deleteGroupHistory(group_id);
    db->removeUserFromGroup(group_id);
    db->deleteGroup(group_id);

    json result, data;
    result["type"] = DELETE_GROUP;
    data["result"] = "delete successfully";
    result["data"] = data;
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("Deletion message sending failed",confd,DELETE_GROUP);
    }
    else{
        std::cout<<confd<<" Deletion message sent successfully\n\n";
    }
}

void Server::getGroupMembers(int confd,json &request){
    ID group_id = request["group_id"];

    auto members = db->getGroupMember(group_id);
    std::vector<json>data;
    while(members.count() > 0){
        json res;
        auto row=members.fetchOne();
        res["user_id"] =(ID)row.get(GROUP_MEMBER_USER_ID);
        data.push_back(res);
    }
    json result;
    result["type"]=GET_GROUP_MEMBERS;
    result["data"]=data;
    int success = sendjson(confd, result);

    if(success == -1)
    {
        Error("Members info sending failed",confd,GET_GROUP_MEMBERS);
    }
    else{
        std::cout<<confd<<" Members info sent successfully\n\n";
    }
}

void Server::createGroup(int confd,json &request)
{
    if(request["group_description"]==request["null"])
        request["group_description"]="";
    ID res=db->createGroup(request["group_name"],request["user_id"],request["group_description"]);
    json result;
    result["type"]=CREATE_GROUP;
    if(res==-1)
    {
        std::cout<<"group name exists\n";
        Error("group name exists",confd,CREATE_GROUP);
    }
    else{
        db->addUserToGroup(res, request["user_id"]);
        json data;
        data["group_id"]=res;
        result["data"]=data;
        sendjson(confd,result);
        std::cout<<"create group successfully\n";
    }
}

void Server::searchGroup(int confd,json &request)
{
    std::string keyword=(std::string)request["keyword"];
    ID id=0;
    if(keyword.length()<=6)
    for(auto c:keyword)
    if(isdigit(c))
        id=id*10+c-'0';
    else {
        id=0;
        break;
    }

    auto groups=db->searchGroup(id,keyword);
    std::vector<json>res;
    while(groups.count()>0)
    {
        auto row=groups.fetchOne();
        json group;
        group["group_id"]=(ID)row.get(BASIC_GROUP_DATA_GROUP_ID);
        group["group_name"]=row.get(BASIC_GROUP_DATA_GROUP_NAME);
        group["group_desciption"]=row.get(BASIC_GROUP_DATA_GROUP_DESCRIPTION);
        res.push_back(group);
    }
    json result;
    result["type"]=SEARCH_GROUP;
    result["data"]=json(res);
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("search failed",confd,SEARCH_GROUP);
    }
    else{
        std::cout<<confd<<" search group successfully\n\n";
    }
}

void Server::addGroup(int confd,json &request)
{
    ID user_id=request["user_id"];
    ID group_id=request["group_id"];
    if (request["message"]==request["null"])
        request["message"]=(std::string) "My id is "+std::to_string(user_id);
    std::string message=(std::string)request["message"];

    int res=db->createGroupRequest(user_id,group_id,message);
    if(res==-1)
    {
        Error("you have already requested",confd,ADD_GROUP);
    }
    else if(res==-2)
    {
        Error("you are already in this group",confd,ADD_GROUP);
    }    
    else{
        json result;
        result["type"]=ADD_GROUP;
        json data;
        data["result"]="send successfully";
        result["data"]=data;
        sendjson(confd,result);
    }   
}

void Server::getMemberRequest(int confd,json &request)
{
    ID group_id=request["group_id"];
    auto requests=db->getGroupRequest(0,group_id);
    std::vector<json>res;
    while(requests.count()>0)
    {
        auto row=requests.fetchOne();
        json request;
        request["member_id"]=(ID)row.get(GROUP_REQUEST_USER_ID);
        request["group_id"]=group_id;
        request["message"]=row.get(GROUP_REQUEST_REQUEST_MESSAGE);
        res.push_back(request);
    }
    json result;
    result["type"]=GET_MEMBER_REQUEST;
    result["data"]=json(res);
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("get failed",confd,GET_MEMBER_REQUEST);
    }
    else{
        std::cout<<confd<<" get member request successfully\n\n";
    }
}

void Server::acceptMember(int confd,json &request)
{
    ID member_id=request["member_id"];
    ID group_id=request["group_id"];
    bool accept=(bool)request["accept"];
    db->deleteGroupRequest(member_id,group_id);

    if(!accept)
    {
        json result;
        result["type"]=ACCEPT_MEMBER;
        json data;
        data["result"]="refuse successfully";
        result["data"]=data;
        sendjson(confd,result);
        return;
    }

    db->addUserToGroup(group_id,member_id);
    json result;
    result["type"]=ACCEPT_MEMBER;
    json data;
    data["result"]="accept successfully";
    result["data"]=data;
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("accept failed",confd,ACCEPT_MEMBER);
    }
    else{
        std::cout<<confd<<" accept member successfully\n\n";
    }
}

void Server::deleteMember(int confd,json &request)
{
    ID member_id=request["member_id"];
    ID group_id=request["group_id"];
    ID owner_id = (ID)db->getBasicGroupData(group_id).get(BASIC_GROUP_DATA_OWNER), member_confd = (int)db->getUserStatus(member_id).get(USER_STATUS_HANDLE); 
    auto owner_status = db->getUserStatus(owner_id);
    if(member_id == owner_id){
        Error("failed: you are the owner, you can not quit!",confd,DELETE_MEMBER);
        return;
    }
    if(confd != member_confd && ((bool)owner_status.get(USER_STATUS_IS_ONLINE) == false || confd != (int)owner_status.get(USER_STATUS_HANDLE))){
        Error("failed: you do not have the authority!",confd,DELETE_MEMBER);
        return;
    }

    db->removeUserFromGroup(group_id,member_id);
    json result;
    result["type"]=DELETE_MEMBER;
    json data;
    data["result"]="delete successfully";
    result["data"]=data;
    int success=sendjson(confd,result);
    if(success==-1)
    {
        Error("delete failed",confd,DELETE_MEMBER);
    }
    else{
        std::cout<<confd<<" delete member successfully\n\n";
    }
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
    auto groups=db->getGroupsOfAUser(user_id);
    std::vector<json>data;
    while(groups.count()>0)
    {
        json group;
        ID group_id;
        group["group_id"]=group_id=(ID)groups.fetchOne().get(GROUP_MEMBER_GROUP_ID);
        auto row=db->getBasicGroupData(group_id);
        group["group_name"]=(std::string)row.get(BASIC_GROUP_DATA_GROUP_NAME);
        group["owner_id"]=(ID)row.get(BASIC_GROUP_DATA_OWNER);
        group["description"]=(std::string)row.get(BASIC_GROUP_DATA_GROUP_DESCRIPTION);

        data.push_back(group);
    }
    json result;
    result["type"]=GET_GROUPS;
    result["data"]=data;
    sendjson(confd,result);

    sendGroupUnreadMessage(confd,user_id);
}