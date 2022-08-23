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
    connect(sock0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    connect(sock1, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    json message, data;
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
    // 10000 add 10001
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1011,
    "data":{
        "from_id":10000,
        "user_id":10001,
        "accept":true
    }
})";
    // 10001 accept 10000
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1004,
    "data":{
        "user_id":10000
    }
})";
    // get 10000's all friends
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1002,
    "data":{
        "user_id":10000,
        "to_id":10001,
        "time":"2022-01-01 01:31:00",
        "content":"hello"
    }
})";
    // 10000 send private message to 10001
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1002,
    "data":{
        "user_id":10000,
        "to_id":10001,
        "time":"2022-01-01 01:32:00",
        "content":"I know you're online"
    }
})";
    // 10000 send private message to 10001
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1013,
    "data":{
        "user_id":10001
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);
    // 10001 logout

    final_message = R"({
    "type":1002,
    "data":{
        "user_id":10000,
        "to_id":10001,
        "time":"2022-01-01 01:33:00",
        "content":"hey, quickly reply"
    }
})";
    // 10000 send private message to 10001
    send(sock0, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1002,
    "data":{
        "user_id":10000,
        "to_id":10001,
        "time":"2022-01-01 01:34:00",
        "content":"are you kidding me?"
    }
})";
    // 10000 send private message to 10001
    send(sock0, final_message.c_str(), final_message.size(), 0);
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
    "type":1004,
    "data":{
        "user_id":10001
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1006,
    "data":{
        "user_id":10001,
        "from_id":10000,
        "begin_time":"2022-01-01 01:30:00",
        "end_time":"2022-01-01 02:34:00"
    }
})";
    // 10001 get history message from 10000
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1006,
    "data":{
        "user_id":10001,
        "from_id":10000,
        "begin_time":"2022-01-01 01:32:00",
        "end_time":"2022-01-01 02:34:00"
    }
})";
    // 10001 get history message from 10000
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    final_message = R"({
    "type":1004,
    "data":{
        "user_id":10001
    }
})";
    send(sock1, final_message.c_str(), final_message.size(), 0);
    usleep(100000);

    getchar();

    close(sock0);
    close(sock1);

    return 0;
}