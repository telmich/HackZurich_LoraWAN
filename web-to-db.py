#!/usr/bin/env python3

import urllib
import psycopg2

from http.server import BaseHTTPRequestHandler, HTTPServer



# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    # not used, just a sample
    def do_GET(self):
        # Send response status code
        self.send_response(200)

        # Send headers
        self.send_header('Content-type','text/html')
        self.end_headers()

        # Send message back to client
        message = "Hello world!"
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"))
        return

    def do_POST(self):
        length = int(self.headers['Content-Length'])
        post_data = urllib.parse.parse_qs(self.rfile.read(length).decode('utf-8'))

        # Print on stdout
        print(post_data)

        # And insert into the db
        self.insert_xml(post_data)

    def insert_xml(self, data):
        try:
            conn = psycopg2.connect("dbname=hackzurich")
        except Exception as e:
            print("DB Insert failed: %s" % e)




if __name__ == '__main__':
    server_address = ('0.0.0.0', 8000)
    httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
    print('running server...')
    httpd.serve_forever()
