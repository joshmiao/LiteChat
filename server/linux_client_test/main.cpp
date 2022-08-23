#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "json.hpp"
#include <unistd.h>
#include <cstdlib> 

using json = nlohmann::json;

#define buffersize 1024

int main(){
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
  
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    serv_addr.sin_port = htons(2333); 
    connect(sock0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    connect(sock1, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    json message, data;
    std::string final_message;

    message["type"] = 1001;
    data["username"] = "dqwdqd";
    data["pwd"] = "00000000000000000000000000000000";
    data["email"] = "ne#1231.com";
    data["birthday"] = "2001-08-02";
    message["data"] = data;
    final_message = to_string(message);
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    message["type"] = 1000;
    data["email"] = "ne#1231.com";
    data["pwd"] = "00000000000000000000000000000000";
    message["data"] = data;
    final_message = to_string(message);
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    message["type"] = 1001;
    data["username"] = "abab";
    data["pwd"] = "00000000000000000000000000000000";
    data["email"] = "2333.com";
    data["birthday"] = "2001-08-02";
    message["data"] = data;
    final_message = to_string(message);
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    message["type"] = 1000;
    data["user_id"] = 10001;
    data["pwd"] = "00000000000000000000000000000000";
    message["data"] = data;
    final_message = to_string(message);
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    
    final_message = R"({
    "type":1008,
    "data":{
        "keyword":"10001"
    }
})";
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1009,
    "data":{
        "user_id":10000,
        "to_id":10001
    }
})";
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1010,
    "data":{
        "user_id":10001
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1011,
    "data":{
        "from_id":10000,
        "user_id":10001,
        "accept":0
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1009,
    "data":{
        "user_id":10000,
        "to_id":10001
    }
})";
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    
    final_message = R"({
    "type":1011,
    "data":{
        "from_id":10000,
        "user_id":10001,
        "accept":1
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1012,
    "data":{
        "user_id":10001,
        "friend_id":10000
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1009,
    "data":{
        "user_id":10000,
        "to_id":10001
    }
})";
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1011,
    "data":{
        "from_id":10000,
        "user_id":10001,
        "accept":1
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1004,
    "data":{
        "user_id":10000
    }
})";
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1002,
    "data":{
        "user_id":10000,
        "to_id":10001,
        "time":"2022-01-01",
        "content":"hello"
    }
})";
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);


//     final_message = R"({
//     "type":1006,
//     "data":{
//         ""
//     }
// })";
//     send(sock1, final_message.c_str(), final_message.size(), 0);
//     usleep(100000);


    getchar();

    close(sock0);
    close(sock1);

    return 0;
}