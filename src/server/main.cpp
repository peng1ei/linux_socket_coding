#include "day01/tcp_server.h"
#include "day02/process_server.h"
#include "day02/pthread_server.h"

int main(int argc, char* argv[]) {
    // 单进程服务器
    //day01::run(argc, argv);

    // 多进程版并发服务器
    //day02::process_server_run(argc, argv);

    // 多线程版并发服务器
    day02::pthread_server_run(argc, argv);

    return 0;
}