#! /usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright © 2023 wirano <git@wirano.me>
#
# Distributed under terms of the MIT license.


import socketserver 

class TCPHandler(socketserver.BaseRequestHandler):

    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024).strip()
        print("{} wrote:".format(self.client_address[0]))
        print(self.data)

if __name__ == "__main__":
    HOST, PORT = "10.42.0.1", 9000

    with socketserver.TCPServer((HOST, PORT), TCPHandler) as server:
        # Activate the server; this will keep running until you
        # interrupt the program with Ctrl-C
        server.serve_forever()

