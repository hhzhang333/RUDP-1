#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

#include "event.h"
#include "rudp.h"
#include "rudp_api.h"
#include "getaddr.h"
#include "sockaddr6.h"


struct rudp_packet_t {
    struct rudp_hdr header;
    char data[RUDP_MAXPKTSIZE];
} __attribute__((packed));

int receiveDataCallback(int fd, void *arg);

// TODO Question : est-ce que avoir des variables globales suffit dans notre cas ? -> on n'est censé ne gérer qu'une seule socket par programme.
//		utililsation d'un flag qui dit si une fonction à déjà été enregistré pour ce programme et du coup refuse d'en changer? 
// ---> mieux : flag pour dire qu'une socket à déjà était ouverte, et du coup on renvoit un NULL
int (*handler_receive)(rudp_socket_t, struct sockaddr_in6 *, void *, int);
int (*handler_event)(rudp_socket_t, rudp_event_t, struct sockaddr_in6 *);
int socket_open = 0;

/* 
 * rudp_socket: Create a RUDP socket. 
 * May use a random port by setting port to zero. 
 */

rudp_socket_t rudp_socket(int port) {

	if(socket_open != 1){
		int s = socket (AF_INET, SOCK_DGRAM, 0);
		if(s==-1){
			printf("Error while opening the socket. Stop sending.\n");
			return 0;
		}

		int p = port;

		// if port = 0, the function should choose randomly a port number.
		if(p == 0){
			srand(time(NULL));
			p = rand()%60000 + 5000; // port between 5000 and 65000
		}

		struct sockaddr_in s_receiver;
		s_receiver.sin_family = AF_INET;
		s_receiver.sin_addr.s_addr = htonl(INADDR_ANY);
		s_receiver.sin_port = htons(p);

		if(bind(s, (struct sockaddr *) &s_receiver, sizeof(s_receiver)) == -1)
		{
			printf("Error while binding the socket\n");
			return NULL;
		}


		if(event_fd(s,receiveDataCallback, s, "receiveDataCallback") < 0) {
			printf("Error while registering the callback function of the socket.\n");
			return NULL;
		}

		rudp_socket_t rudp_socket = (rudp_socket_t) s;

		return rudp_socket;
	}
	else{
		printf("A socket has already been opened\n");
		return NULL
	}
}

/* 
 *rudp_close: Close socket 
 */ 

int rudp_close(rudp_socket_t rsocket) {
	return 0;
}

/* 
 *rudp_recvfrom_handler: Register receive callback function 
 */ 

int rudp_recvfrom_handler(rudp_socket_t rsocket, 
			  int (*handler)(rudp_socket_t, struct sockaddr_in6 *, 
					 void *, int)) {
	handler_receive = handler;
	return 0;
}

/* 
 *rudp_event_handler: Register event handler callback function 
 */ 
int rudp_event_handler(rudp_socket_t rsocket, 
		       int (*handler)(rudp_socket_t, rudp_event_t, 
				      struct sockaddr_in6 *)) {
	handler_event = handler;
	return 0;
}


/* 
 * rudp_sendto: Send a block of data to the receiver. 
 */

int rudp_sendto(rudp_socket_t rsocket, void* data, int len, struct sockaddr_in6* to) {
	
	return 0;
}

int receiveDataCallback(int fd, void *arg) {

	rudp_socket_t *rudp_socket = (rudp_socket_t*) arg;

	struct sockaddr_in sender;
	int addr_size = sizeof(sender);

	struct rudp_packet_t rudp_receive;
	memset(&rudp_receive, 0x0, sizeof(struct rudp_packet_t));

    int bytes = recvfrom(fd, &rudp_receive, sizeof (rudp_receive), 0, (struct sockaddr*) &sender, (socklen_t*) & addr_size);

    //Verifications
    if (bytes <=0){
    	printf("Error while receiving the data\n");
    	return -1;
    }
    if (rudp_receive.header.version != RUDP_VERSION){
    	printf("Wrong RUDP version\n");
    	return -1;    	
    }

    switch(rudp_receive.header.type) {
    	case RUDP_DATA:

    		return;

    	case RUDP_ACK:


    		return;

    	case RUDP_SYN:


    		return;

    	case RUDP_FIN:


    		return;

    }
}