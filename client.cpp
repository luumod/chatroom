#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int clientFd;

void call_back(int sig){
    if (sig == SIGINT){
        close(clientFd);
        std::cout<<"断开连接"<<std::endl;
        exit(0);
    }
}

int main(){
    signal(SIGINT,call_back);
    //1. 创建socket
    clientFd = socket(AF_INET,SOCK_STREAM,0);
    if (clientFd == -1){
        std::cout<<"创建socket失败!"<<std::endl;
        exit(-1);
    }
    std::cout<<"创建socket成功"<<std::endl;
    //2. 协议地址簇
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //网络字节序
    addr.sin_port = htons(8888); //端口号

    //3. 连接服务器：clientFd建立客户端与服务器之间的连接，所有的发送与接收数据都是由这个clientFd来控制进行
    int r = connect(clientFd,(sockaddr*)&addr,sizeof(addr));
    if (clientFd == -1){
        std::cout<<"连接服务器失败!"<<std::endl;
        exit(-1);
    }
    std::cout<<"连接服务器成功"<<std::endl;
    
    //4. 通信
    std::string buff;
    char recv_buff[1024];
    while (true){
        std::cout<<"请输入要发送给服务器的数据: ";
        std::getline(std::cin,buff);
        r = send(clientFd,buff.c_str(),buff.size(),0);
        std::cout<<"发送" << r <<"字节数据到服务器."<<std::endl;

        r = recv(clientFd,recv_buff,sizeof(recv_buff),MSG_DONTWAIT); //非阻塞接收
        if (r > 0){
            std::cout<<"从服务器返回信息: "<<recv_buff<<std::endl; 
        }
    }
    return 0;
}