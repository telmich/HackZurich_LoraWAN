#!/usr/bin/env python3

import urllib
import psycopg2
import websocket
import xml.etree.ElementTree as ET
from http.server import BaseHTTPRequestHandler, HTTPServer
import re


from websocket import create_connection
ws = create_connection("wss://home-safety-visual.eu-gb.mybluemix.net/alarmsocket")
ws.send("Hello, World")
# result =  ws.recv()
# print "Received '%s'" % result
ws.close()


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

        # Print on stdout
        print(post_data)

        # And insert into the db
        self.insert_xml(post_data)

        # Send to Martin / port 8001

        # Sendo to dashboard
        self.to_dashboard(post_data)

    def to_dashboard(self, data):

        dev = self.devEUI(data)
        text = self.payload(data)

        # Working lora node
        if dev == "9CD90BB52B6A1D01":
            key, value = text.split("=")
            ws = websocket.create_connection("wss://home-safety-visual.eu-gb.mybluemix.net/%s" % (key))

            ws.send("%s:%s" % (dev, text))
            ws.close()

    def devEUI(self, data):
        root = ET.fromstring(data)
        return root[1].text

    def payload_hex(self, data):
        root = ET.fromstring(data)
        return root[7].text

    def payload(self, data):
        myhex = self.payload_hex(data)
        return bytes.fromhex(myhex).decode('utf-8')

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
