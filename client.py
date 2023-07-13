import socket

IP = "127.0.0.1"
PORT = 8080
ADDRESS = (IP, PORT)
SIZE = 1024
FORMAT = "utf-8"

def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(ADDRESS)
    print("Connected to Server.")

if __name__ == "__main__":
    main()