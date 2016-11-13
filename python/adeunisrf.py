#!/usr/bin/env python3

# Send lora packet to node-red when being triggered by postgresql
# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-02
# GPLv3+



import binascii
import lorautil
import logging

#              L A T I T U D E        L O N G I TU DE
# status temp  deg min sec +hem   deg min  min  min
# 9e       17  46  58  204  0       009 02 320  a5 9a   0e e7
# 1        2   3   4   5   6       ?  8   9  10  11 12   13 14


# on display: 46 58 122N   09 02 207E
#             46.58 204    09.02 320
# sodaq: lat=46.9699219 lon=9.0391764
# google:    46.969943,     9.038999

log = logging.getLogger("adeunis")
log.setLevel(logging.DEBUG)

known_devices = [ "0018B20000000C58", "0018B20000000C37", "0018B20000000C59", "0018B20000000CD0" ]

def get_gps(deveui, payload):
    res = []
    if not int(payload[0:2], 16) & (2**7):
        return res

    try:
        lat_deg = float(payload[4:6])
        lat_min = float(payload[6:8])
        lat_sec = float(payload[8:11])
        # lat_frac_sec = float(payload[10:11])

        # lat = lat_deg + lat_min/60.0 + lat_sec/3600.0 + lat_frac_sec/36000.0
        lat = lat_deg + lat_min/60.0 + lat_sec/60000.0

        lon_deg = float(payload[12:15])
        lon_min = float(payload[15:17])
        lon_sec = float(payload[17:20])
        # lon_frac_sec = float(payload[19:20])
        # lon = lon_deg + lon_min/60.0 + lon_sec/3600.0 + lon_frac_sec/36000.0
        lon = lon_deg + lon_min/60.0 + lon_sec/60000.0

        pos = ":lat={:.6f} lon={:.6f}".format(lat, lon)
        res = [ deveui + pos ]

    except ValueError as e:
        log.error("GPS decode error: {}:{} {}".format(deveui, payload, e))

    return res

# sodaq : lat=46.9924235 lon=9.0734456
# garmin:  46 59.548 -- 9 04.360
# adeunis: 46 59 539 -- 9 04 350

def get_temp(deveui, payload):
    res = []

    if int(payload[0:2], 16) & (2**7):
        res = [ deveui + ":temperature=" + str(binascii.a2b_hex(payload)[1])]

    return res

def decode_adeunis(pkg):
    data = pkg.split(":")

    deveui = data[0]
    payload = data[1]

    res = []

    # Only handle known devices
    if not deveui in known_devices:
        return res

    res += get_temp(deveui, payload)
    res += get_gps(deveui, payload)

    return res

def nodered_adeunisrf(provider, data):
    res = decode_adeunis(data)
    if not res:
        return

    for d in res:
        lorautil.nodered_send(provider, d)

if __name__ == '__main__':
    conns = lorautil.pg_conn_notify()

    while True:
        lorautil.pg_wait_for_pkg(conns, nodered_adeunisrf)
