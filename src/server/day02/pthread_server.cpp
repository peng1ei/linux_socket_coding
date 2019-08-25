//
// Created by pl on 19-8-25.
//

#include "pthread_server.h"
#include "../core/warp/warp_socket.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <iostream>

namespace day02 {
    // 自定义数据结构
    typedef struct SockInfo {
        int fd;
        struct sockaddr_in addr;
        pthread_t id;
    } SockInfo;

    // 子线程处理函数
    void* worker(void* arg)
    {
        char ip[64];
        char buf[1024];
        SockInfo* info = (SockInfo*)arg;
        // 通信
        while(true) {
            printf("Client IP: %s, port: %d\n",
                   inet_ntop(AF_INET, &info->addr.sin_addr.s_addr, ip, sizeof(ip)),
                   ntohs(info->addr.sin_port));
            int len = read(info->fd, buf, sizeof(buf));
            if(len == -1) {
                perror("read error");
                pthread_exit(NULL);
            }
            else if(len == 0) {
                printf("客户端已经断开了连接\n");
                close(info->fd);
                break;
            }
            else {
                printf("recv buf: %s\n", buf);
                write(info->fd, buf, len);
            }
        }
        return NULL;
    }

    int pthread_server_run(int argc, char *argv[]) {
        if (argc < 2) {
            std::cout << "eg: ./a.out server_port" << std::endl;
            exit(1);
        }

        int port = atoi(argv[1]);

        // 创建套接字
        int lfd = wsock::Socket(AF_INET, SOCK_STREAM, 0);

        // 初始化服务器 sockaddr_in
        struct sockaddr_in serv_addr;
        socklen_t serv_len = sizeof(serv_addr);
        memset(&serv_addr, 0, serv_len);
        serv_addr.sin_family = AF_INET;                   // 地址族
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // 监听本机所有的IP
        serv_addr.sin_port = htons(port);            // 设置端口

        // 绑定IP和端口
        wsock::Bind(lfd, (struct sockaddr*)&serv_addr, serv_len);

        // 设置同时监听的最大个数
        wsock::Listen(lfd, 36);

        std::cout << "Start accept ......" << std::endl;

        // 该服务器最多能支持 256 个客户端访问
        int i = 0;
        SockInfo info[256];
        // 规定 fd == -1
        for(i=0; i<sizeof(info)/sizeof(info[0]); ++i)
        {
            info[i].fd = -1;
        }

        socklen_t cli_len = sizeof(struct sockaddr_in);
        while (true) {
            // 选择一个没有被使用的，最小的数组元素
            // TODO 如果某个已连接的客户端断开连接，归还一个 fd（info） 时，该如何处理
            for(i=0; i<256; ++i) {
                if(info[i].fd == -1) {
                    break;
                }
            }
            if(i == 256) {
                break;
            }

            // 主线程 - 等待接受连接请求
            info[i].fd = accept(lfd, (struct sockaddr*)&info[i].addr, &cli_len);

            // 创建子线程 - 通信
            pthread_create(&info[i].id, NULL, worker, &info[i]);

            // 设置线程分离
            // 子线程结束后，资源被自动回收
            pthread_detach(info[i].id);
        }

        close(lfd);

        // 只退出主线程
        // 子线程仍在处理，如果没结束的话
        pthread_exit(NULL);

        return 0;
    }
}
