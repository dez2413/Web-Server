#!/usr/bin/env python3

import socket
import subprocess
import os
import sys
import time
import requests
import queue
import threading
import random

HERE = os.path.dirname(os.path.realpath(__file__))

#if len(sys.argv) != 2 or not os.path.isdir(sys.argv[1]) or not os.path.join(sys.argv[1], "Makefile"):
#    print("Usage: ./autograder /path/to/student/makefile/directory")
#    exit(1)

#cmd = f"`cd {sys.argv[1]}; make server`"

# Start the server
#server_process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

score = 0

# TEST 1: serve the home page 1000 times

address = "http://localhost:8080/"

num_sequential_requests = 1000

err_counter = 0
for i in range(num_sequential_requests):
    try:
        r = requests.get(address)
        assert r.status_code == 200
        assert "<html>" in r.text
        assert "</html>" in r.text
        assert "<body>" in r.text
        assert "</body>" in r.text
    except Exception as err:
        err_counter += 1

print(f"TEST 1: Of {num_sequential_requests} sequential requests, {err_counter} failed") 
score += (1 - (err_counter/num_sequential_requests)) * 30

# Test 2: serve the home page to 1000 concurrent requests
address = "http://localhost:8080/index.html"

num_concurrent_requests = 1000

def gethomepage(success_queue):
    address = "http://localhost:8080/index.html"
    try:
        #print("asking...")
        r = requests.get(address)
        assert r.status_code == 200
        assert "<html>" in r.text
        assert "</html>" in r.text
        assert "<body>" in r.text
        assert "</body>" in r.text
        success_queue.put(True)
    except Exception as err:
        #print(f"Failed to get homepage: {type(err)}: {err}\nStatus Code: {r.status_code}")
        success_queue.put(False)

success_queue = queue.Queue()
request_threads = []
for i in range(num_concurrent_requests):
    t = threading.Thread(target = gethomepage, args=(success_queue,))
    request_threads.append(t)
    t.daemon
    t.start()

for i in request_threads:
    i.join()

err_counter = 0
while True:
    try:
        if not success_queue.get(timeout=1):
            err_counter += 1
    except queue.Empty:
        break
print(f"TEST 2: Of {num_concurrent_requests} concurrent requests, {err_counter} failed") 
score += (1 - (err_counter/num_concurrent_requests)) * 30

# Test 3 -- Correctly Issue Error Conditions
errors = 0
successes = 0

tests = [
    ["/google", "POST", "405"],
    ["/google", "DELETE", "405"],
    ["/google", "OPTIONS", "405"],
    ["/multiply", "POST", "400"],
    ["/multiply", "GET", "405"],
    ["/database.php", "DELETE", "403"],
    ["/database.php", "GET", "405"],    
    ["/", "POST", "405"],
    ["/index.html", "POST", "405"],
    ["/favicon.ico", "GET", "404"],
]

def try_requesting(a):
    addr, m, c = a
    address = "http://localhost:8080" + addr
    try:
        if m == "POST":
            return requests.post(address).status_code    == int(c)
        elif m == "DELETE": 
            return requests.delete(address).status_code  == int(c)
        elif m == "GET":
            return requests.get(address).status_code     == int(c)
        elif m == "OPTIONS":
            return requests.options(address).status_code == int(c)
        else:
            assert False
    except:
        return False

for t in tests:
    if try_requesting(t):
        successes += 1
    else:
        print(f"Error! {t}")
        errors += 1

print(f"TEST 3: {successes} correct responses and {errors} incorrect error responses")
score += (successes / (errors + successes)) * 10


# Test 4 -- Correctly Redirect
errors = 0
successes = 0
address = "http://localhost:8080/google"

try:
    r = requests.get(address,allow_redirects=False)
    if r.status_code == 301:
        if r.headers["location"] == "https://google.com":
            successes += 1
        else:
            print("Error /google redirected to " + r.headers["location"])
            assert False
    else:
        print("GET /google status code was " + str(r.status_code))
        assert False
except Exception as err:
    errors += 1

print(f"TEST 4: {successes} correct responses and {errors} incorrect redirects")
score += (successes / (errors + successes)) * 10

# Test 5 -- Correctly Multiply
errors = 0
successes = 0

address = "http://localhost:8080/multiply"

for i in range(10):
    a = random.randint(-90,99)
    b = random.randint(-90,9999)
    answ = a*b
    r = requests.post(address,f"a={a}&b={b}")
    if str(answ) in r.text:
        successes += 1
    else:
        errors += 1

print(f"TEST 5: {successes} correct responses and {errors} incorrect /multiply requests")
score += (successes / (errors + successes)) * 20

# Report score:
print(f"UNDERGRAD SCORE: {round(score,2)}/100")

# Cleanup
#server_process.kill()
