#!/usr/bin/env python3

# Send lora packet to node-red when being triggered by postgresql
# Nico Schottelius <nico.schottelius -at- ungleich.ch>
# 2016-11-02
# GPLv3+


import lorautil
import logging

log = logging.getLogger("notify")
log.setLevel(logging.DEBUG)

if __name__ == '__main__':
    conns = lorautil.pg_conn_notify()

    while True:
        lorautil.pg_wait_for_pkg(conns, lorautil.nodered_send)
