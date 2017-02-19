#!/usr/bin/env python3

import urllib
import psycopg2
import websocket
from http.server import BaseHTTPRequestHandler, HTTPServer
import re
import json
import pprint
import lorautil
import base64

# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(length).decode('utf-8')

        print(post_data)

        payload = self.payload_hex(post_data)
        deveui = self.get_deveui(post_data)

        # Try to decode to unicode
        try:
            payload = self.data_to_unicode(payload)
        except UnicodeDecodeError:
            pass

        print("deveui/payload: {}:{}".format(deveui, payload))

        # And insert into the db
        lorautil.db_insert_json("ttn", post_data, payload, deveui)
        lorautil.db_notify("ttn", payload, deveui)

    def payload_hex(self, data):
        mydict = lorautil.jsonToDict(data)
        return mydict['payload_raw']

    def data_to_unicode(self, myhex):
        return base64.b64decode(myhex).decode('utf-8')

    def get_deveui(self, data):
        mydict = lorautil.jsonToDict(data)
        return mydict['hardware_serial']


if __name__ == '__main__':
    server_address = ('0.0.0.0', 7000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    httpd.serve_forever()
