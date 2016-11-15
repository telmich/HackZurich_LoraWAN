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

logging.basicConfig(format='%(levelname)s: %(message)s')

class RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(length).decode('utf-8')

        try:
            if json.loads(post_data)['action'] == "connected":
                self.send_24h_gps()

        except Exception as e:
            log.error("Got unknown request: {} {}".format(post_data, e))

    def send_24h_gps(self):
        db = lorautil.DB.gps_query()

        for record in db:
            lorautil.nodered_send("gps-plain", record)

if __name__ == '__main__':
    log = logging.getLogger(__name__)
    server_address = ('0.0.0.0', 1900)
    httpd = HTTPServer(server_address, RequestHandler)
    httpd.serve_forever()
