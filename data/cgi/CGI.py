#!/usr/bin/env python3

import cgi
import os

# for key, value in os.environ.items():
#     print(f"{key}: {value}")

# print("-" * 80)

from pprint import pprint

print("Content-type: text/html\n")
print("<html><head><title>Test CGI</title></head><body>")
print("<h1>Test CGI</h1>")
print("<p>Hello World!</p>")

# form = cgi.FieldStorage()
# if "name" in form:
#     print("<p>Your name is: {}</p>".format(form["name"].value))


print("</body></html>")
