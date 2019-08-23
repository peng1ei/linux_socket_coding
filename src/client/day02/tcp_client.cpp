//
// Created by pl on 19-8-23.
//

#include "tcp_client.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

namespace day02 {
    int run(int argc, const char* argv[]) {
        if (argc < 2) {
            std::cout << "eg: ./a.out port" << std::endl;
            exit(1);
        }

        int port = atoi(argv[1]);

        // 创建套接字
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            perror("socket error");
            exit(1);
        }

        // 连接服务器
        struct sockaddr_in serv;
        memset(&serv, 0, sizeof(serv));

        serv.sin_family = AF_INET;
        serv.sin_port = htons(port);
        //serv.sin_addr.s_addr = htonl(); // 整形IP
        inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);

        int ret = connect(fd, (struct sockaddr*)&serv, sizeof(serv));
        if (ret == -1) {
            perror("connect error");
            exit(1);
        }

        // 通信
        while (true) {
            // 先发送数据
            char buf[1024];

            std::cout << "请输入要发送的数据: " << std::endl;

            fgets(buf, sizeof(buf), stdin);
            write(fd, buf, strlen(buf));

            // 等待接收数据
            int len = read(fd, buf, sizeof(buf));
            if (len == -1) {
                perror("read error");
                exit(1);
            } else if (len == 0) {
                perror("服务器端关闭了连接\n");
                break;
            } else {
                std::cout << buf << std::endl;
            }
        }

        close(fd);

        return 0;
    }
}