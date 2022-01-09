#include "sonic_led_socket.h"
#include "sonic_led_socket.cpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>

int main(){
    std::cout << "build server" << std::endl;
    silicon_one::sonic_led_socket server;
    server.init();
    char buf_test[1024] = {0};
    int i = 1;
    uint64_t test = 50;
    char* recv_state;
    while(1){
        // recv_state = silicon_one::sonic_led_socket::recv_state();

        sprintf(buf_test, "%lld|%d \n", test, i);
        silicon_one::sonic_led_socket::send_state(buf_test);
        i++;
        sleep(5);
    }
}