# Helper functions for various Lora receivers
# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-02
# GPLv3+

import psycopg2
import json
import logging

log = logging.getLogger("lorautil")
log.setLevel(logging.DEBUG)

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

def jsonToDict(self, data):
    return json.loads(data)
