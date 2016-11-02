#!/usr/bin/env python3

import ssl
import websocket
import json
import os
import psycopg2

class Loriot():
    def __init__(self):
        ws = websocket.WebSocket()

    def connect(self):
        tokenurl = os.environ['LORIOT_URL']
        self.ws = websocket.create_connection(tokenurl, sslopt={"cert_reqs": ssl.CERT_NONE})

    def listen(self):
        while True:
            result = self.ws.recv()
            print(result)

            jdata = lorautil.jsonToDict(result)
            eui = self.devEUI(jdata)
            lorautil.insert_json("loriot", result, deveui=eui)

    def devEUI(self, data):
        return data['EUI']

    def get_payload(self, data):
        return ""
        # return data['EUI']

if __name__ == '__main__':
    l = Loriot()
    l.connect()
    l.listen()
