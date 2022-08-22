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
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    int sock2 = socket(AF_INET, SOCK_STREAM, 0);
  
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    serv_addr.sin_port = htons(2333); 
    connect(sock1, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    connect(sock2, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    json message, data;
    std::string final_message;

    message["type"] = 1001;
    data["username"] = "dqwdqd";
    data["pwd"] = "00000000000000000000000000000000";
    data["email"] = "ne#1231.com";
    data["birthday"] = "2001-08-02";
    message["data"] = data;
    final_message = to_string(message);
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(1000000);

    message["type"] = 1000;
    data["email"] = "ne#1231.com";
    data["pwd"] = "00000000000000000000000000000000";
    message["data"] = data;
    final_message = to_string(message);
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(1000000);

    message["type"] = 1001;
    data["username"] = "abab";
    data["pwd"] = "00000000000000000000000000000000";
    data["email"] = "2333.com";
    data["birthday"] = "2001-08-02";
    message["data"] = data;
    final_message = to_string(message);
    send(sock2, final_message.c_str(), final_message.size(), 0);
    usleep(1000000);

    message["type"] = 1000;
    data["user_id"] = 10001;
    data["pwd"] = "00000000000000000000000000000000";
    message["data"] = data;
    final_message = to_string(message);
    send(sock2, final_message.c_str(), final_message.size(), 0);
    usleep(1000000);
    
    getchar();

    close(sock1);
    close(sock2);

    return 0;
}