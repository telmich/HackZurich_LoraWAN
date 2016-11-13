# node.py
# Simple Python client to show node activity from ttn MQTT brooker with credentials
# Author: R.Schimmel
# www.schimmel-bisolutions.nl
# first install paho.mqtt.client: pip install paho-mqtt
#
# Enhanced for lorawan in Digital Glarus by Nico Schottelius (nico.schottelius -at- ungleich.ch)
#

import paho.mqtt.client as mqtt
import psycopg2
import json
import base64
import os
import logging

import lorautil



def on_connect(client,userdata,rc):
    log.debug("Connected with result code:"+str(rc))
    # subscribe for all devices of user
    client.subscribe('+/devices/+/up')


def on_message(client,userdata,msg):
    myjson = msg.payload.decode('utf-8')
    mydict = json.loads(myjson)
    deveui = mydict['dev_eui']

    try:
        payload = base64.b64decode(mydict['payload']).decode('utf-8')
    except UnicodeDecodeError as e:
        log.info("Cannot decode packet as utf-8")
        payload = mydict['payload']

    log.info("Message received: {}: {}".format(deveui, payload))
    lorautil.db_insert_json("ttn", myjson, payload, deveui)
    lorautil.db_notify("ttn", payload, deveui)

def on_log(client,userdata,level,buf):
    log.debug("message:" + msg)
    log.debug("userdata:" + str(userdata))

def on_disconnect(client, userdata, rc):
    log.debug("reconnecting...")
    client.reconnect()

if __name__ == '__main__':
    logging.root.setLevel(logging.INFO)
    logging.basicConfig(format='%(levelname)s: %(message)s')

    log = logging.getLogger(__name__)

    mqttc= mqtt.Client()
    mqttc.on_connect=on_connect
    mqttc.on_message=on_message
    mqttc.on_disconnect=on_disconnect

    log.debug("Connecting to ttn")
    username=os.environ['ttn_user']
    password=os.environ['ttn_password']

    mqttc.username_pw_set(username, password)
    mqttc.connect("staging.thethingsnetwork.org",1883,10)

    # and listen to server
    log.info("Listening via mqtt")
    run = True
    while run:
        mqttc.loop()
