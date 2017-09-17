#!/usr/bin/python
# -*- coding:utf-8 -*-

import time
import threading,signal
import json, urllib2
from random import choice
#from utils import *
#from config import *

params_data = {}
#api_url = "http://192.168.115.100:8085/api.php"
#api_url = "http://192.168.115.70:8085/api.php"
api_url = "http://139.129.201.216:9011/api.php"
thread_num = 100

def load_test_params_data(param_path):
    for line in open(param_path):
        if line[0] != '#':
            key, val = line.split('=')
            params_data[key] = "api=" + urllib2.quote(val)
        #print key

def post_param_data():
    key = choice(params_data.keys())
    #key = "Login.account"
    try:
        response = urllib2.urlopen(api_url, params_data[key], timeout=3).read()
        print key + "---" + response
        #print response
    except Exception, e:
        print str(e)
    time.sleep(1)

is_exit = False
def worker(index):
    while not is_exit:
        try:
            post_param_data()
        except:
            pass

def handler(signum, frame):
    global is_exit
    is_exit = True
    print "receive a signal to exit"

if __name__ == "__main__":
    signal.signal(signal.SIGINT, handler)
    signal.signal(signal.SIGTERM, handler)
    load_test_params_data("./params")
    #test()
    threads = []
    for i in xrange(0, thread_num):
        t = threading.Thread(target=worker, args=(i,))
        t.setDaemon(True)
        threads.append(t)
        t.start()
    
    while 1:
        alive = False
        for i in xrange(0, thread_num):
            alive = alive or threads[i].isAlive()
        if not alive:
            break