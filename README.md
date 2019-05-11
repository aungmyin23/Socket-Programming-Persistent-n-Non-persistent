# Socket-Programming-Persistent-n-Non-persistent

Server:
	gcc server.c
	./a.out [Connection selection]
	
	eg. ./a.out 0

Connection selection: 	0 for non-persistent client. 
			1 for persistent client.
 
Client:
	gcc persistent_client.c or gcc non_persistent_client.c
	./a.out [IP] 

	eg. ./a.out 127.0.0.1