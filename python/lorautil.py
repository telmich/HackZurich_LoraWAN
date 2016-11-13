# Helper functions for various Lora receivers
# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-02
# GPLv3+

import psycopg2
import json
import logging

import select
import psycopg2
import psycopg2.extensions
import sys
import time
import websocket


log = logging.getLogger("lorautil")

dbname="lorawan"


def db_notify(provider, payload='', deveui=''):
    notify="{}:{}".format(deveui, payload)
    log.debug("Notify: {} {}".format(provider, notify))
    try:
        conn = psycopg2.connect("dbname={}".format(dbname))
        cursor = conn.cursor()

        cursor.execute("select pg_notify (%s, %s)",  (provider, notify))
        cursor.connection.commit()
    except Exception as e:
        log.error("DB Notify failed: %s" % e)

def db_insert_json(provider, data, payload='', deveui=''):

    try:
        conn = psycopg2.connect("dbname={}".format(dbname))
        cursor = conn.cursor()
        cursor.execute("insert into packets values (DEFAULT, DEFAULT, %s, %s, %s, %s)",  (provider, data, payload, deveui))
        cursor.connection.commit()

        conn.close()
    except Exception as e:
        log.error("DB Insert failed: %s" % e)

def jsonToDict(data):
    return json.loads(data)

def nodered_from_stdin():
    provider = sys.argv[1]
    for line in sys.stdin:
        print("{} -> {}".format(provider, line))
        nodered_send(provider,line)
        time.sleep(0.1)


def nodered_send(provider, data):
    ws = websocket.create_connection("ws://localhost:1880/{}".format(provider))
    ws.send("%s" % data)
    ws.close()

channels = [ "loriot", "swisscom", "ttn" ]

def pg_conn_notify():
    conns = []
    for channel in channels:
        conn = psycopg2.connect("dbname={}".format(dbname))
        conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)

        curs = conn.cursor()
        curs.execute("LISTEN {};".format(channel))

        conns.append(conn)
        log.debug("Waiting for notifications on channel {}".format(channel))

    return conns

def pg_wait_for_pkg(conns, callback):
    readable, writable, exceptional = select.select(conns,[],[])

    for conn in readable:
        conn.poll()
        while conn.notifies:
            notify = conn.notifies.pop(0)
            log.debug("Got NOTIFY: {} {} {}".format(notify.pid, notify.channel, notify.payload))

            callback(notify.channel, notify.payload)
