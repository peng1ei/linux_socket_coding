//
// Created by pl on 19-8-23.
//
#include "tcp_server.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

namespace day01 {

    // 服务器端
    int run(int argc, char* argv[]) {
        std::cout << "day01::run(argc, argv)" << std::endl;

        // 创建监听的套接字
        int lfd = socket(AF_INET, SOCK_STREAM, 0);
        if (lfd == -1) {
            perror("socket error");
            exit(1);
        }

        // lfd 和本地的 IP:Port 绑定
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(8888);

        // 将套接字绑定到 （IP:Port）
        int ret = bind(lfd, (struct sockaddr*)&server, sizeof(server));
        if (ret == -1) {
            perror("bind error");
            exit(1);
        }

        // 设置能同时发起连接的最大客户数
        ret = listen(lfd, 20);
        if (ret == -1) {
            perror("listen error");
            exit(1);
        }

        // 等待并接收连接请求
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int cfd = accept(lfd, (struct sockaddr*)&client, &len);
        if (cfd == -1) {
            perror("accept error");
            exit(1);
        }

        std::cout << "accept successful !!!" << std::endl;

        // 一直通信，等待客户端的连接
        while (true) {
            // 先接收数据
            char buf[1024] = {0};
            int len = read(cfd, buf, sizeof(buf));
            if (len == -1) {
                perror("read error");
                exit(1);
            } else if (len == 0) {
                std::cout << " 客户端已经断开了连接 " << std::endl;
                break;
            } else {
                std::cout << "recv buf: " << buf << std::endl;

                // 转换：小写 -> 大写
                for (int i = 0; i < len; ++i) {
                    buf[i] = toupper(buf[i]);
                }

                std::cout << "send buf: " << buf << std::endl;
                write(cfd, buf, len);
            }

        }

        close(lfd);
        return 0;
    }

}