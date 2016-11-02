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

        try:
            payload = self.dataToString(post_data)
        except UnicodeDecodeError:
            payload = ""

        try:
            deveui = self.dataToDevEUI(post_data)
        except UnicodeDecodeError:
            deveui = ""

        print("deveui/payload: {}:{}".format(deveui, payload))

        # And insert into the db
        lorautil.db_insert_json("swisscom", post_data, payload, deveui)
        lorautil.db_notify("swisscom", payload, deveui)

    def jsonToDict(self, data):
        return json.loads(data)

    def dictToPayload(self, thedict):
        return thedict['DevEUI_uplink']['payload_hex']

    def hexToString(self, myhex):
        return bytes.fromhex(myhex).decode('utf-8')

    def dataToString(self, data):
        mydict = self.jsonToDict(data)
        payload = self.dictToPayload(mydict)
        return self.hexToString(payload)

    def dataToDevEUI(self, data):
        mydict = self.jsonToDict(data)
        eui = mydict['DevEUI_uplink']['DevEUI']
        return eui

    def devEUI(self, data):
        root = ET.fromstring(data)
        return root[1].text

    def payload_hex(self, data):
        root = ET.fromstring(data)
        return root[7].text

    def payload(self, data):
        myhex = self.payload_hex(data)
        return bytes.fromhex(myhex).decode('utf-8')


if __name__ == '__main__':
    server_address = ('0.0.0.0', 8000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    httpd.serve_forever()
