#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#define PORT 1337
#define ERR_EXIT_CODE 1

typedef struct sockaddr_in saddress_in;
typedef struct sockaddr saddress;

int main(int argc, char const *argv[])
{
	int socket_fd, valread;
	saddress_in server_addr;
    double toreceive;
    int toreceiveint;
    double tosend;
    int tosendint;
    int ext;
	//char* tosend;
    //char* tmp=malloc(sizeof(char)*1024);

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		exit(ERR_EXIT_CODE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	// Convert address from text to binary form
	if(inet_aton(&argv[1], &server_addr.sin_addr) == 0)
	{
        
		printf("\nInvalid address/Address not supported \n");
		exit(ERR_EXIT_CODE);
	}
	if (connect(socket_fd, (saddress *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		exit(ERR_EXIT_CODE);
	}
    ext=0;
    while(!ext)
    {
        printf("Please enter the first number: ");
        scanf("%lf",&tosend);
        //tosendint=strlen(tmp);
        //tosend=malloc((tosendint+1)*sizeof(char));
        //strncpy(tosend,tmp,tosendint);
        write(socket_fd , &tosend, sizeof(double) );
        printf("Please enter the second number: ");
        scanf("%lf",&tosend);
        write(socket_fd , &tosend, sizeof(double) );
        do
        {
            printf("Operation codes: 0 = +, 1 = -,2 = *, 3 = /, 4 = mod, 5 = ^\n");
            printf("Please enter operation code: ");
            scanf("%d",&tosendint);
            if(tosendint<0 || tosendint>5)
            {
                printf("Invalid Operation Code. Please try again.\n");
            }
        }
        while(tosendint<0 || tosendint>5);
        write(socket_fd , &tosendint, sizeof(int) );
        //write(socket_fd , tosend , sizeof(char)*(strlen(tosend)+1) );
        //printf("hatespeach sent\n");

        valread = read( socket_fd , &toreceiveint, sizeof(int));
        valread = read( socket_fd , &toreceive, sizeof(double));
        if(toreceiveint==0)
        {
            printf("The result is %lf\n", toreceive);
        }else if(toreceiveint==2)
        {
            printf("+-Infinity (the limit that is)\n");
        }
        else if(toreceiveint==1)
        {
            printf("Invalid format\n");
        }
        else
        {
            printf("Undefined\n");
        }
        printf("Would you like to continue? (0 = Yes, any other number = No)\n");
        scanf("%d", &ext);
        write(socket_fd , &ext, sizeof(int) );
    }

    //free(tosend);
    //free(tmp);
	return 0;
}


// https://www.youtube.com/watch?v=Ag1AKIl_2GM
