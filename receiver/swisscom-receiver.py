#!/usr/bin/env python3

import urllib
import psycopg2
import websocket
from http.server import BaseHTTPRequestHandler, HTTPServer
import re
import json
import pprint

# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    # not used, just a sample
    def do_GET(self):
        # Send response status code
        self.send_response(200)

        # Send headers
        self.send_header('Content-type','text/html')
        self.end_headers()

        # Send message back to client
        message = "Hello world!"
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"))
        return

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
        self.insert_json("swisscom", post_data, payload, deveui)

        # Sendo to dashboard
#        self.to_dashboard(post_data)

    def to_dashboard(self, data):
        dev = self.devEUI(data)
        text = self.payload(data)

        ws = websocket.create_connection("wss://home-safety-visual.eu-gb.mybluemix.net/data")
        ws.send("%s" % data)
        ws.close()

        ws = websocket.create_connection("wss://home-safety-visual.eu-gb.mybluemix.net/rawmessage")
        ws.send("%s:%s" % (dev, text))
        ws.close()

        # Working lora node
        if dev == "9CD90BB52B6A1D01":
            try:
                key, value = text.split("=")
                print("Trying to send: %s:%s" % (key, value))
                ws = websocket.create_connection("wss://home-safety-visual.eu-gb.mybluemix.net/%s" % (key))
                ws.send("%s" % (value))
                ws.close()

            except ValueError:
                print("Cannot split: %s" % (text))


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

    def insert_json(self, provider, data, payload='', deveui=''):
        try:
            conn = psycopg2.connect("dbname=lorawan")
            cursor = conn.cursor()
            cursor.execute("insert into packets values (DEFAULT, DEFAULT, %s, %s, %s, %s)",  (provider, data, payload, deveui))
            cursor.connection.commit()
            conn.close()
        except Exception as e:
            print("DB Insert failed: %s" % e)


    def insert_xml(self, data):
        try:
            conn = psycopg2.connect("dbname=hackzurich")
            cursor = conn.cursor()
            cursor.execute("insert into data (packet) values (%s)",  (data, ))
            cursor.connection.commit()
            conn.close()
        except Exception as e:
            print("DB Insert failed: %s" % e)


if __name__ == '__main__':
    server_address = ('0.0.0.0', 8000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    httpd.serve_forever()
