import select
import psycopg2
import psycopg2.extensions

conn = psycopg2.connect("dbname=lorawan")
conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)

curs = conn.cursor()
curs.execute("LISTEN test;")

print("Waiting for notifications on channel test")
while True:
    if select.select([conn],[],[]) == ([],[],[]):
        print("Timeout")
    else:
        conn.poll()
        while conn.notifies:
            notify = conn.notifies.pop(0)
            print("Got NOTIFY: {} {} {}".format(notify.pid, notify.channel, notify.payload))
