#!/usr/bin/env python3

import urllib
import psycopg2
import websocket
from http.server import BaseHTTPRequestHandler, HTTPServer
import re
import json
import pprint
import lorautil

# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(length).decode('utf-8')

        payload = self.payload_hex(post_data)
        deveui = self.get_deveui(post_data)

        # Try to decode to unicode
        try:
            payload = self.data_to_unicode(payload)
        except UnicodeDecodeError:
            pass

f        print("deveui/payload: {}:{}".format(deveui, payload))

        # And insert into the db
        lorautil.db_insert_json("swisscom", post_data, payload, deveui)
        lorautil.db_notify("swisscom", payload, deveui)

    def payload_hex(self, data):
        mydict = lorautil.jsonToDict(data)
        return mydict['DevEUI_uplink']['payload_hex']

    def data_to_unicode(self, myhex):
        return bytes.fromhex(myhex).decode('utf-8')

    def get_deveui(self, data):
        mydict = lorautil.jsonToDict(data)
        eui = mydict['DevEUI_uplink']['DevEUI']
        return eui


if __name__ == '__main__':
    server_address = ('0.0.0.0', 8000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    httpd.serve_forever()
