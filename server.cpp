#include <iostream>
#include <functional>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>

#define MAX_SIZE 1024

static int clientFd[1024];

//  多线程通信
void* thread_function(void* id);

class STcpServer{
public:
    STcpServer(){
		init();


		// 协议地址簇
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //点分十进制ip地址转化为网络字节序
		addr.sin_port = htons(8888); //端口号

		_socket();
		_bind();
		_listen();

    }
    ~STcpServer(){}

	int exec(){
		return _accept();
	}

	void disconnect(){
		for (int i = 0;i < MAX_SIZE;i++){
			if (clientFd[i]!=-1){
				close(clientFd[i]);
			}
		}
		close(serverFd);
		std::cout<<"断开连接"<<std::endl;
	}
private:
	void init(){
		for (int i = 0;i < MAX_SIZE;i++){
			clientFd[i]=-1;
		}
	}
	
	// 创建socketFd
    void _socket(){
		serverFd = socket(AF_INET,SOCK_STREAM,0);
		if (serverFd == -1){
			std::cout<<"创建socket失败!"<<std::endl;
			exit(-1);
		}
		std::cout<<"创建socket成功"<<std::endl;
	}

	// 绑定
	void _bind(){
		//3. 绑定
		int r = bind(serverFd,(sockaddr*)&addr,sizeof(addr));
		if (r == -1){
			std::cout<<"绑定失败! "<<strerror(errno)<<std::endl;
			exit(-1);
		}
		std::cout<<"绑定成功"<<std::endl;
	}

	// 监听
	void _listen(){
		//4. 监听：serverFd是监听套接字，并不是与客户端通信的套接字
		int res = listen(serverFd,100);
		if (res == -1){
			std::cout<<"监听失败!"<<std::endl;
			exit(-1);
		}
		std::cout<<"监听成功"<<std::endl;
	}

	// 接受客户端连接
	int _accept(){
		sockaddr_in caddr{}; //接受客户端的协议地质簇
		socklen_t len = sizeof(caddr);
		for (int i =0;i<MAX_SIZE;i++){

			//阻塞在这里，等待多个客户端连接

			clientFd[i] = accept(serverFd,(sockaddr*)&caddr,&len);
			if (clientFd[i] == -1){
				std::cout<<"服务器崩溃!"<<std::endl;
				return -1;
			}
			std::cout<<"接受客户端连接成功: "<< clientFd[i] << ", "<<inet_ntoa(caddr.sin_addr)<<", "<< caddr.sin_port << std::endl; //ntoa：转换为点分十进制
			//每接受一个客户端连接，就启动一个线程
			int result = pthread_create(&pthreads[i],NULL,thread_function,(void*)(uintptr_t)i);
			if (result != 0){
				std::cout<<"创建线程失败! "<< result<<std::endl;
				return -1;
			}
		}
		return 0;
	}

	
private:
	int serverFd{};
	sockaddr_in addr{};
	pthread_t pthreads[MAX_SIZE];

}server;




void signal_fun(int sig){
	server.disconnect();
}

int main(){
	std::function<void(int)> handle = signal_fun;
	signal(SIGINT,signal_fun);
    return server.exec();
}

void* thread_function(void* id){
	char buff[1024]{};
	int idx = (int)(uintptr_t)id;
	while (true){
		int r = recv(clientFd[idx],buff,sizeof(buff),0);
		if (r > 0){
			buff[r] = '\0';
			printf("%d, %s\n",idx,buff);
		}
	}
}