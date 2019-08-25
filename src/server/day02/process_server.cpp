//
// Created by pl on 19-8-25.
//

#include "process_server.h"
#include "../core/warp/warp_socket.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <iostream>

// 进程回收函数
void recyle(int num) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        std::cout << "child died, pid = " << pid << std::endl;
    }
}

namespace day02 {

    int process_server_run(int argc, char *argv[]) {
        if (argc < 2) {
            std::cout << "eg: ./a.out port" << std::endl;
            exit(1);
        }

        int port = atoi(argv[1]);

        // 创建套接字
        int lfd = wsock::Socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in serv_addr;
        socklen_t  serv_len = sizeof(serv_addr);

        // 初始化服务器 sockaddr_in
        memset(&serv_addr, 0, serv_len);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听本机所有的 IP
        serv_addr.sin_port = htons(port);

        // 绑定 IP和端口
        wsock::Bind(lfd, (struct sockaddr*)&serv_addr, serv_len);

        // 设置同时监听的最大个数
        wsock::Listen(lfd, 36);

        std::cout << "Start accept ......" << std::endl;

        // 使用信号回收子进程 PCB
        struct sigaction act;
        act.sa_handler = recyle;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        sigaction(SIGCHLD, &act, NULL);

        struct sockaddr_in client_addr;
        socklen_t child_len = sizeof(client_addr);

        while (true) {
            // 父进程接收连接请求
            // accept 阻塞的时候被信号中断，处理信号对应的操作之后
            // 回来之后不阻塞了(why???)，直接返回 -1，这时候 errno == EINTR
//            int cfd = accept(lfd, (struct sockaddr*)&client_addr, &child_len);
//            while (cfd == -1 && errno == EINTR) {
//                cfd = accept(lfd, (struct sockaddr*)&client_addr, &child_len);
//            }
            int cfd = wsock::Accept(lfd, (struct sockaddr*)&client_addr, &child_len);

            std::cout << "connect sucessful" << std::endl;

            // 创建子进程
            pid_t pid = fork();
            if (pid == 0) {
                close(lfd);

                // 通信
                char ip[64];
                while (true) {
                    // client ip:port
                    std::cout << "client IP: " << inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)) <<
                        ", Port: " << ntohs(client_addr.sin_port) << std::endl;

                    char buf[1024];
                    int len = read(cfd, buf, sizeof(buf));
                    if (len == -1) {
                        std::cout << "Read error" <<std::endl;
                        exit(1);
                    } else if (len == 0) {
                        std::cout << "客户端断开了连接" << std::endl;
                        close(cfd); // 可以不必调用该句，进程结束后，文件描述符自动清除
                        break;
                    } else {
                        std::cout << "recv buf: " << buf << std::endl;

                        // 转换：小写 -> 大写
                        for (int i = 0; i < len; ++i) {
                            buf[i] = toupper(buf[i]);
                        }

                        write(cfd, buf, len);
                    }
                }
                // 结束子进程
                return 0;
            } else if (pid > 0) {
                // 父进程
                close(cfd);
            }

        }

        close(lfd);
        return 0;
    }

}