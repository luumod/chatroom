#include <iostream>
#include <functional>
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

void signal_fun(int sig);


class STcpClient{
public:
    STcpClient(){
        init();
		_socket();
        _connect();

    }
    ~STcpClient(){}

	int exec(){
		//4. 通信
        std::string buff;
        char recv_buff[1024];
        while (true){
            std::cout<<"请输入要发送给服务器的数据: ";
            std::getline(std::cin,buff);
            int r = send(clientFd,buff.c_str(),buff.size(),0);
            std::cout<<"发送" << r <<"字节数据到服务器."<<std::endl;

            r = recv(clientFd,recv_buff,sizeof(recv_buff),MSG_DONTWAIT); //非阻塞接收
            if (r > 0){
                std::cout<<"从服务器返回信息: "<<recv_buff<<std::endl; 
            }
        }
        return 0;
	}

	void disconnect(){
		close(clientFd);
		std::cout<<"断开连接"<<std::endl;
	}
private:
    //协议地址簇
	void init(){
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //网络字节序
        addr.sin_port = htons(8888); //端口号
	}
	
	// 创建socketFd
    void _socket(){
        clientFd = socket(AF_INET,SOCK_STREAM,0);
        if (clientFd == -1){
            std::cout<<"创建socket失败!"<<std::endl;
            exit(-1);
        }
        std::cout<<"创建socket成功"<<std::endl;
	}

	// 绑定
	void _connect(){
		//3. 连接服务器：clientFd建立客户端与服务器之间的连接，所有的发送与接收数据都是由这个clientFd来控制进行
        int r = connect(clientFd,(sockaddr*)&addr,sizeof(addr));
        if (clientFd == -1){
            std::cout<<"连接服务器失败!"<<std::endl;
            exit(-1);
        }
        std::cout<<"连接服务器成功"<<std::endl;
	}
private:
	int clientFd{};
	sockaddr_in addr{};

}client;

int main(){
    signal(SIGINT,signal_fun);
    return client.exec();
}

void signal_fun(int sig){
    if (sig == SIGINT){
        client.disconnect();
    }
}
