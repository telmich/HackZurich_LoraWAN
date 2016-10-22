# node.py
# Simple Python client to show node activity from ttn MQTT brooker with credentials
# Author: R.Schimmel
# www.schimmel-bisolutions.nl
# first install paho.mqtt.client: pip install paho-mqtt
#
import paho.mqtt.client as mqtt
import psycopg2
import json
import base64
import os

#Call back functions

# gives connection message
def on_connect(client,userdata,rc):
    print("Connected with result code:"+str(rc))
    # subscribe for all devices of user
    client.subscribe('+/devices/+/up')

# gives message from device
def on_message(client,userdata,msg):
    myjson = msg.payload.decode('utf-8')
    mydict = json.loads(myjson)
    deveui = mydict['dev_eui']
    payload = base64.b64decode(mydict['payload']).decode('utf-8')

    print("{}: {}".format(deveui, payload))
    insert_json("ttn", myjson, payload, deveui)

def on_log(client,userdata,level,buf):
    print("message:" + msg)
    print("userdata:" + str(userdata))

def insert_json(provider, data, payload='', deveui=''):
    try:
        conn = psycopg2.connect("dbname=lorawan")
        cursor = conn.cursor()
        cursor.execute("insert into packets values (DEFAULT, DEFAULT, %s, %s, %s, %s)",  (provider, data, payload, deveui))
        cursor.connection.commit()
        conn.close()
    except Exception as e:
        print("DB Insert failed: %s" % e)

if __name__ == '__main__':
    mqttc= mqtt.Client()
    mqttc.on_connect=on_connect
    mqttc.on_message=on_message

    print("Connecting to ttn")
    username=os.environ['ttn_user']
    password=os.environ['ttn_password']

    mqttc.username_pw_set(username, password)
    mqttc.connect("staging.thethingsnetwork.org",1883,10)

    # and listen to server
    print("Listening via mqtt")
    run = True
    while run:
        mqttc.loop()
