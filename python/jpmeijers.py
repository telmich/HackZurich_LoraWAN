#!/usr/bin/env python3

# Send lora packet to node-red when being triggered by postgresql
# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-02
# GPLv3+


import binascii
import struct
import lorautil
import logging
import sys

log = logging.getLogger(__name__)
log.setLevel(logging.DEBUG)

known_devices = [ "0004A30B001C6613" ]

def convert_gps_from_stdin():
    for line in sys.stdin:
        res = get_gps("", line)
        if res:
            print(res[0][1:])

# Parsing code from jpmeiyers
def get_gps(deveui, payload):
    res = []
    data = binascii.a2b_base64(payload)

    if(len(data)!=9):
        return res

    parsed = struct.unpack("<BBBBBBBBB", data)
    values = {}
    values['lat'] = (parsed[0]*(2**16) + parsed[1]*(2**8) + parsed[2])/16777215.0*180.0-90
    values['lon'] = (parsed[3]*(2**16) + parsed[4]*(2**8) + parsed[5])/16777215.0*360.0-180
    values['alt'] = parsed[6]*(2**8) + parsed[7]
    values['acc'] = parsed[8]/10.0

    pos = ":lat={lat:.6f} lon={lon:.6f} alt={alt:.6f} acc={acc:.2f}.format(**values)
    res = [ deveui + pos ]

    return res

def decode(pkg):
    data = pkg.split(":")

    deveui = data[0]
    payload = data[1]

    res = []

    # Only handle known devices
    if not deveui in known_devices:
        return res

    res += get_gps(deveui, payload)

    return res

def nodered(provider, data):
    res = decode(data)
    if not res:
        return

    for d in res:
        lorautil.nodered_send(provider, d)

if __name__ == '__main__':
    conns = lorautil.pg_conn_notify()

    while True:
        lorautil.pg_wait_for_pkg(conns, nodered)
