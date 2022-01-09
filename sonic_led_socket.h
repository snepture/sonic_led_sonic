
#ifndef _SONIC_LED_SOCKET_H_
#define _SONIC_LED_SOCKET_H_
#include "sonic_led_socket.h"


namespace silicon_one{
    class sonic_led_socket{
        public:
            static int listenfd;
            static int conn[];
            static int count_c;
            // static char* state;
            static bool init();
            static bool socket_connect();
            static bool send_state(char* buf);
            static char* recv_state();
            static bool mul_conn(int sock);

    };
};
#endif
