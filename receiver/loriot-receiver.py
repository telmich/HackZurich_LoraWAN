#!/usr/bin/env python3

import ssl
import websocket
import json
import os
import psycopg2
import lorautil

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

            try:
                payload = get_payload(jdata)
            except Exception:
                payload = ""

            lorautil.db_insert_json("loriot", result, payload=payload, deveui=eui)
            lorautil.db_notify("loriot", payload=payload, deveui=eui)

    def devEUI(self, data):
        return data['EUI']

    def get_payload(self, data):
        return bytes.fromhex(data['payload']).decode('utf-8')

if __name__ == '__main__':
    l = Loriot()
    l.connect()
    l.listen()
