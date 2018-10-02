#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import socket, struct
import pickle
import gzip
import base64

class Targets:
    def __init__(self, datasetPath, compressionLevel):
        self.pickleFile = None
        
        if datasetPath != None:
            self.pickleFile  = open(datasetPath,"w")

    def parse(self, frame, jsonstr):
        try:
            dct = json.loads(jsonstr)
        except ValueError:
            print(jsonstr)
            return None
        
        dct['frame'] = base64.b64encode(frame).decode("utf-8")
        if self.pickleFile != None:
            str = json.dumps(dct)
            self.pickleFile.write(str)
        return dct

class Client:
    def __init__(self, ip='127.0.0.1', port=8000, datasetPath=None, compressionLevel=0):
        print('Trying to connect to DeepGTAV')
        
        self.targets = Targets(datasetPath, compressionLevel)

        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.connect((ip, int(port)))
        except Exception as e:
            print('ERROR: Failed to connect to DeepGTAV')
            print(e)
        else:
            print('Successfully connected to DeepGTAV')

    def sendMessage(self, message):
        jsonstr = message.to_json().encode('utf-8')
        try:
            self.s.sendall(len(jsonstr).to_bytes(4, byteorder='little'))
            self.s.sendall(jsonstr)
        except Exception as e:
            print('ERROR: Failed to send message. Reason:', e)
            return False
        return True

    def recvMessage(self):
        frame = self._recvall()
        if not frame: 
            print('ERROR: Failed to receive frame')     
            return None
        data = self._recvall()
        if not data: 
            print('ERROR: Failed to receive message')       
            return None
        return self.targets.parse(frame, data.decode('utf-8'))

    def _recvall(self):
        #Receive first size of message in bytes
        data = b""
        while len(data) < 4:
            packet = self.s.recv(4 - len(data))
            if not packet: return None
            data += packet
        size = struct.unpack('I', data)[0]

        #We now proceed to receive the full message
        data = b""
        while len(data) < size:
            packet = self.s.recv(size - len(data))
            if not packet: return None
            data += packet
        return data

    def close(self):
        self.s.close()
