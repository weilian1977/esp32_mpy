#!/usr/bin/env python3
# -*- coding: utf-8 -*-

' Utility entity '


import time
import uos,os

def is_dir_exists(path, search_dir):
    if type(path) != str:
        return None
    if type(search_dir) != str:
        return None

    cwd = uos.getcwd()
    uos.chdir(path)
    dirs = uos.listdir()
    uos.chdir(cwd)
    for d in dirs:
        if d == search_dir:
            return True
    return False

def if_file_exists(path):
    if(type(path)!=str):
        return False
        print("path is not str")
    uos.chdir("/")
    dirs=path.split("/")
    #print(path)
    #print(dirs)
    for index in range (1,(len(dirs))-1):
        if not is_dir_exists(uos.getcwd(),dirs[index]):
            uos.chdir("/")
            return False
        uos.chdir(dirs[index])
    files=uos.listdir()
    uos.chdir("/")
    if dirs[len(dirs)-1] in files:
        return True
    return False
    #print("file not exists")

def if_dir_exists(path):
    if(type(path)!=str):
        return False
        print("path is not str")
    uos.chdir("/")
    dirs=path.split("/")
    for index in range (1,(len(dirs))):
        if not is_dir_exists(uos.getcwd(),dirs[index]):
            uos.chdir("/")
            return False
        uos.chdir(dirs[index])
    uos.chdir("/")
    return True

# If path is "/a/b/" using absulute directory
# else if path is "./a/b" using relative directory
def make_dir(path):
    if type(path) != str:
        return False

    cwd = uos.getcwd()
    if ('/' == path[0]):
        cur_path = "/"
    elif ('.' == path[0]):
        path = path.lstrip('.')
        cur_path = cwd

    uos.chdir(cur_path)
    dirs = path.strip('/').split('/')
    for d in dirs:
        if (d!=''):
            if (not is_dir_exists(cur_path, d)):
                uos.mkdir(d)
                
            uos.chdir(d)
            cur_path = uos.getcwd()

    uos.chdir(cwd)
    return True

def walk_to_dir(path):
    if type(path) != str:
        return False

    if ('/' == path[0]):
        uos.chdir('/')
    elif ('.' == path[0]):
        path = path.lstrip('.')

    dirs = path.strip('/').split('/')
    cur_dir = "/"
    uos.chdir(cur_dir)
    for d in dirs:
        if (d != ''):
            print("cur_dir:%s, d:%s"%(cur_dir, d))
            if not is_dir_exists(cur_dir, d):
                return False
            uos.chdir(d)
            cur_dir = uos.getcwd()

    return True

def caculate_32bit_xor_checksum(data):
    # print("****", data, type(data))
    # if type(data) != bytearray or type(data) != list:
    #    return None

    bytes_len = len(data)
    checksum = bytearray(4)
    checksum[0] = 0
    checksum[1] = 0
    checksum[2] = 0
    checksum[3] = 0
    for i in range(int(bytes_len/4)):
        checksum[0] = checksum[0] ^ data[i*4 + 0]
        checksum[1] = checksum[1] ^ data[i*4 + 1]
        checksum[2] = checksum[2] ^ data[i*4 + 2]
        checksum[3] = checksum[3] ^ data[i*4 + 3]

    if ( bytes_len%4 ):
        for i in range(bytes_len%4):
            checksum[0+i] = checksum[0+i] ^ data[4*int(bytes_len/4) + i]

    return checksum