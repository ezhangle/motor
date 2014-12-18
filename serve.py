#!/usr/bin/python

import http.server

class RequestHandler(http.server.SimpleHTTPRequestHandler):
  def do_GET(self):
    print("url: " + self.path)
    super(RequestHandler, self).do_GET()

server = http.server.HTTPServer(('', 8000), RequestHandler)

server.serve_forever()
