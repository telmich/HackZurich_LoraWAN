#!/usr/bin/env python3

# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-15
# GPLv3+

import urllib
import psycopg2
import websocket
from http.server import BaseHTTPRequestHandler, HTTPServer
import re
import json
import lorautil
import logging


class RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(length).decode('utf-8')

        print(post_data)

        # And insert into the db
        #lorautil.db_insert_json("swisscom", post_data, payload, deveui)
        #lorautil.db_notify("swisscom", payload, deveui)


if __name__ == '__main__':
    server_address = ('0.0.0.0', 1900)
    httpd = HTTPServer(server_address, RequestHandler)
    httpd.serve_forever()
