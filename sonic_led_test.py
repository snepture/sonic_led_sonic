# -*- coding:utf-8 -*-
#!/usr/bin/env python3

import socket
import json
import sys
import asyncio
import select

load_dict = {}
stop = 0
timeout = 3

async def sending(s):
    while 1:
        #python2 & python3
        # try: 
        #     input = raw_input
        # except NameError: pass
        await asyncio.sleep(1)
        rlist, _, _ = select.select([sys.stdin], [], [], timeout)
        if rlist:
            a = sys.stdin.readline()
            if not a:
                continue
            s.sendall(a.encode())

    global stop
    stop = 1

async def listening(s):
    while 1:
        if stop == 1:
            break
        await asyncio.sleep(1)
        server_reply = s.recv(512).decode()
        print("Received from server: {}".format(server_reply))
        if server_reply != "":
            reply_slice = server_reply.split('|', 3)
            slice_id = reply_slice[0]
            ifg_id = reply_slice[1]
            serdes_id = reply_slice[2]
            led_state = reply_slice[3]
            print("slice_id: {}, ifg_id: {}, serdes_id: {}".format(slice_id, ifg_id, serdes_id))
        else:
            continue

        speed = "100G"
        if speed == "100G":
            first_lane_num = int((int(slice_id) * 2 + int(ifg_id)) * (0x100) + int(serdes_id))
        elif speed == "400G":
            first_lane_num = int((slice_id * 2 + ifg_id) * (0x100) + serdes_id)
        else:
            print("error")

        print("First lane: {}".format(first_lane_num))
        
        final_eth = None
        for Eth in load_dict['PORT']:
            if str(first_lane_num) in load_dict['PORT'][Eth]['lanes']:
                final_eth = Eth

        print("reply_slice: {}, first_lane_num: {}, Eth: {}".format(reply_slice, first_lane_num, final_eth))


    


if __name__ == "__main__":
    try: 
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
    except socket.error as e: 
        print ("Error creating socket: %s" % e) 
        sys.exit(1) 
    port = ('127.0.0.1', 7891)
    
    try: 
        s.connect(port) 
    except socket.gaierror as e: 
        print ("Address-related error connecting to server: %s" % e) 
        sys.exit(1) 
    except socket.error as e: 
        print ("Connection error: %s" % e) 
        sys.exit(1) 
    hello = s.recv(512).decode
    print(hello)
    f = open("/Users/yecsong/code/sonic_led_sonic/config_db.json","r")
    load_dict = json.load(f)

    loop = asyncio.get_event_loop()
    tasks = [sending(s), listening(s)]
    loop.run_until_complete(asyncio.wait(tasks))
    loop.close()

    s.close()

