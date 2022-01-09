# -*- coding:utf-8 -*-
#!/usr/bin/env python3

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
port = ('127.0.0.1', 7891)
s.connect(port)

while 1:
    #python2 & python3
    try: 
        input = raw_input
    except NameError: pass
    a = input("Please input: ")
    if not a:
        continue
    s.sendall(a.encode())

    if a == "exit":
        print("exit!")
        break

    server_reply = s.recv(1024).decode()
    print(server_reply)

s.close()

