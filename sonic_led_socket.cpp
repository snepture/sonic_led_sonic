#include "sonic_led_socket.h"
#include <iostream>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>

#define LED_PORT 7891
#define BACKLOG 2
#define MAXSOCKFD 10

namespace silicon_one{

int sonic_led_socket::listenfd = 0;
int sonic_led_socket::count_c = 0;
int sonic_led_socket::conn[] = {0};

bool sonic_led_socket::init(){

    if(listenfd == 0){
        std::thread led_thread(socket_connect);
        led_thread.detach();
    }
    return true;
}

bool sonic_led_socket::socket_connect(){
    sonic_led_socket::listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int nFlags;
    if ((nFlags = fcntl (listenfd, F_GETFL, 0)) < 0)
        return 0;
    if (fcntl (listenfd, F_SETFL, nFlags | O_NONBLOCK) < 0)
        return 0;
    //socket
    if(listenfd == -1){
        std::cout << "Error: socket" << std::endl;
        return 0;
    }
    //bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LED_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cout << "Error: bind" << std::endl;
        return 0;
    }
    //listen
    if(listen(listenfd, BACKLOG)<0){
        std::cout << "Error: listen" << std::endl;
        close(listenfd);
        return 0;
    }

    mul_conn(listenfd);
    return true;
}
//multiple connection
bool sonic_led_socket::mul_conn(int listenfd){
    //accept
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    fd_set connfds;
    
    while(1){
        int con;
        FD_ZERO(&connfds);
        FD_SET(listenfd, &connfds);
        for(int i = 0; i < count_c; i++) {  
            if (conn[i] != 0) {  
                FD_SET(conn[i], &connfds);  
            }  
        }
        if(select(MAXSOCKFD+1, &connfds, NULL, NULL, NULL) > 0){
            if(FD_ISSET(listenfd, &connfds) > 0){
                con = accept(listenfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
                if(con < 0){
                    std::cout << "Error: accept" << std::endl;
                    return 0;
                }
                sonic_led_socket::conn[count_c++] = con;
                std::cout << "New Connection"<< con << std::endl;
                FD_SET(con, &connfds);
            }
            

            for(int i=0; i<count_c; i++){
                int contemp = sonic_led_socket::conn[i];
                char buf[1024] = {0};
                if(FD_ISSET(contemp, &connfds) > 0){
                    int len = recv(contemp, buf, sizeof(buf), 0);
                    std::cout << "Receive:" << buf << std::endl;
                    if (len == 0) {
                        std::cout << "Disconnect connection" << contemp << std::endl;
                        close(contemp);
                        FD_CLR(contemp, &connfds);
                        sonic_led_socket::conn[i] = 0;
                    }
                      
                }
            }
        }
    }
}

bool sonic_led_socket::send_state(char* buf){
    for(int i=0;i<count_c;i++){

        int con = sonic_led_socket::conn[i];
        int sendsize = 0;

        if(buf){
            int sendsize = send(con, buf, sizeof(buf), 0);
        }else{
            std::cout << "No message to send" << std::endl;
        }
        if(sendsize == -1){
            std::cout << "Error: send" << std::endl;
            return 0;
        }
    }

    std::cout << buf << std::endl;
    
    return true;
}

// char* sonic_led_socket::recv_state(){
//     char buf[1024] = {0};
//     while(1){
//         for(int i=0;i<count_c;i++){
//             int con = sonic_led_socket::conn[i];
//             int len = recv(con, buf, sizeof(buf), 0);
//             if (len == 0) {
//                 std::cout << "Disconnect connection" << con << std::endl;
//                 close(con);
//                 FD_CLR(con, &connfds);
//             }
//         }
//     } 
       
//     return buf;
// }

}