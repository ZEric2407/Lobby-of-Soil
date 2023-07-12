server.exe: server.c 
	gcc server.c -o server.exe -lwsock32 -lws2_32