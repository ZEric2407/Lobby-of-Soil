import socket
import tkinter as tkt
import asyncio, sys, select
from time import sleep
from threading import Thread

IP = "127.0.0.1"
PORT = 8080
ADDRESS = (IP, PORT)
SIZE = 1024
FORMAT = "utf-8"
NAME = input("Please enter your name: ")

def receive(socket):
    try:
        msg = socket.recv(SIZE).decode()
        print(msg)
    except Exception as e:
        print("An exception has occurred while receiving message: " + e)

def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(ADDRESS)
    print("Connected to Server.")
    client.send(bytes(NAME, FORMAT))

    while True:
        receive_thread = Thread(target=receive, args=[client])
        receive_thread.start()

        message = "[" + NAME + "]"
        stdin = input("[You]: ")
        if stdin == "--exit--":
            closeSocket(client)
            return
        message += stdin
        client.send(bytes(message, FORMAT))
        sys.stdout.flush
    

def closeSocket(sock):
    sock.close()

if __name__ == "__main__":
    main()