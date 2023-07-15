from tkinter import *
import os
from client import *

USERNAME = ""
IP_ADDRESS = ""

def join(msg_history, name, ip):
    global USERNAME, IP_ADDRESS
    USERNAME = name.get()
    IP_ADDRESS = ip.get()
    if name == "":
        popup = Tk()
        warning = Label(popup, text="Invalid Username").pack()
        return -1
    
    elif IP_ADDRESS == "" or not "".join(IP_ADDRESS.split(".")).isdigit():
        return invalidAddressPopup()
    
    connect(USERNAME, IP_ADDRESS)
    if CLIENT_SOCK == -1:
        invalidAddressPopup()
        return -1
    
    receive_thread = Thread(target=receive, args=[msg_history])
    receive_thread.start()
    print("Thread started.")

def receive(msg_history):
    while True:
        try:
            msg = CLIENT_SOCK.recv(SIZE).decode()
            msg_history.insert(END, msg)
        except Exception as e:
            print("An exception has occurred while receiving message: " + e)

def host(msg_history, name, ip):
    global USERNAME, IP_ADDRESS
    USERNAME = name.get()
    if USERNAME == "":
        popup = Tk()
        warning = Label(popup, text="Invalid Username").pack()
        return -1
    ip.set("127.0.0.1")
    os.startfile("server.exe")
    result = join(msg_history, name, ip)
    return result

def send_msg(user_msg):
    if user_msg.get() == "":
        return
    msg = "[" + USERNAME + "]: " + user_msg.get()
    user_msg.set("")
    CLIENT_SOCK.send(bytes(msg, FORMAT))
    
def invalidAddressPopup():
    popup = Tk()
    Label(popup, text="Invalid IP Address").pack()
    return -1

def main():
    window = Tk()
    window.title("Lobby of Soil")
    window.configure(bg="white")
    window.geometry("800x600")


    creds_frame = Frame(window, width=100, height=20)
    creds_frame.grid(row=0, column=0)
    name_lab = Label(creds_frame, text="Username")
    name_lab.grid(row=0,column=0)
    name = StringVar()
    name_entry = Entry(creds_frame, bd=3, textvariable=name)
    name_entry.grid(row=0, column=1)

    ip_label = Label(creds_frame, text="IP Address")
    ip_label.grid(row=0, column=2)
    ip = StringVar()
    ip_entry = Entry(creds_frame, bd=3, textvariable=ip)
    ip_entry.grid(row=0,column=3)

    join_button = Button(creds_frame, text="Join", command=lambda: join(msg_history, name, ip))
    join_button.grid(row=0,column=4)
    host_button = Button(creds_frame, text="Host", command=lambda: host(msg_history, name, ip))
    host_button.grid(row=0,column=5)


    msg_frame = Frame(window, height=70, width=100)
    msg_frame.grid(row=1, column=0)

    scroll_bar = Scrollbar(msg_frame)
    scroll_bar.grid(row=0,column=1)

    msg_history = Listbox(msg_frame, height = 30, width=70, yscrollcommand=scroll_bar.set)
    msg_history.grid(row=0,column=0)


    user_input_frame = Frame(window, height=10, width=100)
    user_input_frame.grid(row=2, column=0)

    user_msg = StringVar()
    user_msg_field = Entry(user_input_frame, textvariable=user_msg, width=68)
    user_msg_field.grid(row=0, column=0)

    send_button = Button(user_input_frame, text="Send", command=lambda : send_msg(user_msg))
    send_button.grid(row=0, column=1)
    


if __name__ == "__main__":
    main()
    mainloop()