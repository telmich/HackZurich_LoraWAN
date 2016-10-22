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
            jdata = jsonToDict(result)
            eui = devEUI(jdata)
            payload = get_payloan(jdata)


    def jsonToDict(self, data):
        return json.loads(data)

    def devEUI(self, data):
        return data['EUI']

    def get_payload(self, ):
        return data['EUI']

    def insert_json(self, provider, data, payload='', deveui=''):
        try:
            conn = psycopg2.connect("dbname=lorawan")
            cursor = conn.cursor()
            cursor.execute("insert into packets values (DEFAULT, DEFAULT, %s, %s, %s, %s)",  (provider, data, payload, deveui))
            cursor.connection.commit()
            conn.close()
        except Exception as e:
            print("DB Insert failed: %s" % e)


if __name__ == '__main__':
    l = Loriot()
    l.connect()
    l.listen()
