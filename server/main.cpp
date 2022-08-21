#include<Server.h>
#include<Server.cpp>
int main()
{
    Server server(2333);
    server.Start();
    return 0;
}