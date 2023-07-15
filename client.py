import socket
import asyncio, sys, select
from time import sleep
from threading import Thread

PORT = 8080
SIZE = 1024
FORMAT = "utf-8"
CLIENT_SOCK = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

def connect(username, ip):
    address = (ip, PORT)
    name = username
    try: 
        CLIENT_SOCK.connect(address)
        print("Connected to Server.")
    except Exception as e:
        print(e)
        return -1
    CLIENT_SOCK.send(bytes(name, FORMAT))
    return CLIENT_SOCK    

def closeSocket(sock):
    sock.close()

if __name__ == "__main__":
    client = connect("TEST", "127.0.0.1")
    sleep(10)
