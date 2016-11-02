#!/usr/bin/env python3

# Send lora packet to node-red when being triggered by postgresql
# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-02
# GPLv3+

import select
import psycopg2
import psycopg2.extensions
import websocket

channels = [ "loriot", "swisscom", "ttn" ]

def to_nodered(provider, data):
    ws = websocket.create_connection("ws://localhost:1880/{}".format(provider))
    ws.send("%s" % data)
    ws.close()

def wait_for_pkg(conns):

    readable, writable, exceptional = select.select(conns,[],[])

    for conn in readable:
        conn.poll()
        while conn.notifies:
            notify = conn.notifies.pop(0)
            print("Got NOTIFY: {} {} {}".format(notify.pid, notify.channel, notify.payload))
            to_nodered(notify.channel, notify.payload)

if __name__ == '__main__':
    conns = []
    for channel in channels:

        conn = psycopg2.connect("dbname=lorawan")
        conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)

        curs = conn.cursor()
        curs.execute("LISTEN {};".format(channel))

        conns.append(conn)
        print("Waiting for notifications on channel {}".format(channel))

    while True:
        wait_for_pkg(conns)
