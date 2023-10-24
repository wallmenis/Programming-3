#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>

#define FLIGHT_COUNT 10
#define PORT 1337
#define ERR_EXIT_CODE 1

typedef struct sockaddr_in saddress_in;
typedef struct sockaddr saddress;

int main(int argc, char const *argv[])
{
    int i;
    int orcount;
    int avail;
    int rnumber;
    int res;
	int socket_fd, valread;
	saddress_in server_addr;
    orcount=0;
    avail=0;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		exit(ERR_EXIT_CODE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	// Convert address from text to binary form
	if(inet_aton(argv[1], &server_addr.sin_addr) == 0)
	{
        
		printf("\nInvalid address/Address not supported \n");
		exit(ERR_EXIT_CODE);
	}
	if (connect(socket_fd, (saddress *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		exit(ERR_EXIT_CODE);
	}

    //sleep(1);
    //printf("[CLIENT %d ]: Client is running\n", id);
    //srand(time(NULL));
    orcount = rand()%3 + 1;
    //printf("[CLIENT %d ]: Sending number of orders (%d)\n", id, orcount);
    //close(serverfds[READ]);
    write (socket_fd, &orcount ,sizeof(int));
    //printf("[CLIENT %d ]: orcount after sending attempt: %d\n",id , orcount);
    for(i = 0; i < orcount; i++)
    {
        srand(time(NULL));                      //Κανονικά η srand ειναι για την αρχή του προγράμματος αλλά, λόγω
                                                //της ψευδοτυχαιότητάς της βγάζει συνέχεια ίδιες τιμές, άρα
                                                //επαναρχηκοποιούμε για νέα τιμή βάσει χρόνου.
        rnumber = rand()%FLIGHT_COUNT;
        //printf("[CLIENT %d ]: Sending order %d of %d with content %d\n",id, i, orcount, rnumber);
        //close(serverfds[READ]);
        write(socket_fd,&rnumber ,sizeof(int));
        //close(clientfds[WRITE]);
        read(socket_fd, &res, sizeof(int));
    }

	return 0;
}


// https://www.youtube.com/watch?v=Ag1AKIl_2GM
