#!/usr/bin/env python3

# Send lora packet to node-red when being triggered by postgresql
# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-02
# GPLv3+

# Bit 6 = 1 : accelerometer was triggered
#
# Bit 5 = 0 : BTN1 was NOT triggered
#
# Bit 4 = 1 : GPS info is present
#
# Bit 3 = 1 : MAC Down Counter is present
#
# Bit 2 = 1 : MAC up Counter is present
#
# Bit 1 = 1 : Battery voltage information is present
#
# Bit 0 = 0 : RSSI + SNR information is NOT present
#
# * Byte 02 : 1E = 0001 1110 (Temperature in °C, signed in two’s complement) = 30 °C
#
# * Byte 03 : 47 = GPS Latitude, degrees
#
# * Byte 04 : 22 = GPS Latitude, minutes
#
# * Byte 05 : 49 = GPS Latitude, minutes
#
# * Byte 06 : 60 = GPS Latitude, minutes + Hemisphere (0 = North)
#
# * Byte 07 : 00 = GPS Longitude, minutes
#
# * Byte 08 : 83 = GPS Longitude, minutes
#
# * Byte 09 : 18 = GPS Longitude, minutes
#
# * Byte 10 : 70 = GPS Longitude, minutes + Hemisphere (0 = East)
#
# * Byte 11 : none (because Byte 12 is starting with a 0 and not 1, 2 or 3)
#
# * Byte 12 : 08 = Uplink frame counter
#
# * Byte 13 : 08 = Downlink frame counter
#
# * Byte 14 : 0E = MSB Battery voltage )
#
# * Byte 15 : FD = LSB Battery voltage ) è 3.837 V
#
# * Byte 16 : none
#
# * Byte 17 : none

# should be: 46.969943, 9.038999

import binascii


def decode_pkg(pkg):
    return binascii.a2b_hex(s)

def has_gps(pkg):
    return pkg[0] & 0x10

def gpsdata(pkg):
    if not has_gps(pkg):
        return (0,0)


if __name__ == '__main__':
    decode_pkg()
