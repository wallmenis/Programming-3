#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 1337
#define ERR_EXIT_CODE 1
#define CLIENT_NUM 5
#define FLIGHT_COUNT 10

typedef struct sockaddr_in saddress_in;
typedef struct sockaddr saddress;
typedef struct fl{
    int flight_id;
    char* from_city;
    char* to_city;
    int available_seats;
    float price;
}flight;

/*-----------------------------------------------------------------*/ //Δηλώσεις συναρτήσεων

int chkavail(flight*);
void printflight(flight*);
void exithandler();
void freeallmem();
void exitterm();

/*-----------------------------------------------------------------*/ //Global bois

flight flights[FLIGHT_COUNT];
int flightrequests[FLIGHT_COUNT];
int flightsuccess[FLIGHT_COUNT];
float flightprofitsum;
int flightrequestssum;
int flightsuccesssum;
char * city_table[5];

int* tosend;
float* tosendfloat;
int* toreceive;
char* tosendtext;

/*-----------------------------------------------------------------*/ //Η main
int main(int argc, char** argv)
{
    int i,j;
    int ticketcount;
    int orno;

    int socket_fd;
    int client_fd;
    int internalerror;
    saddress_in address;
    int addrlen = sizeof(address);
    int rsize;
    srand(time(NULL));
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
	{
		perror("Creating socket file descriptor failed");
		exit(ERR_EXIT_CODE);
	}
    
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

    internalerror = bind(socket_fd, (saddress*)(&address), addrlen);
    if(internalerror<0)
    {
        perror("Binding socket file descriptor to address failed");
		exit(ERR_EXIT_CODE);
    }

    signal(SIGTERM,exitterm);       //Η όλη ιστορία με το γιατί χρησιμοποιούμε handlers και σήματα είναι γιατί βιαστίκαμε να διαβάσουμε
                                    //όλη την εκφώνηση και υποθέσαμε οτι το πρόγραμμα κλείνει μεχρι κάποιος να πατήσει ctrl+c ή ctrl+d ή
                                    //στην χειρότερη να τερματίστει την διεργασία με kill σκέτο. Τα αφήνουμε καθώς δεν πειράζουν τόσο το
                                    //τελικό αποτέλεσμα.
    signal(SIGINT,exitterm);        //Επειδή τώρα δηλώνουμε την μνήμη και δεν θέλουμε memory leaks, να κλείνει σωστα η εφαρμογή
    signal(SIGQUIT,exitterm);       //στην περίεργη περίπτωση που είναι κανείς γρήγορος να την κλείσει.

    tosend = malloc(sizeof(int));
    tosendfloat = malloc(sizeof(float));
    toreceive =  malloc(sizeof(int));
    tosendtext = calloc(20,sizeof(char));

/*-----------------------------------------------------------------*/ //Αρχηκοποίηση μεταβλητών
    flightrequestssum = 0;
    flightsuccesssum = 0;
    flightprofitsum = 0;
    for (i=0; i<FLIGHT_COUNT; i++)
    {
        flightrequests[i]=0;
        flightsuccess[i]=0;
    }
/*-----------------------------------------------------------------*/ //Δήλωση ονομάτων απο πόλεις.
    city_table[0]=malloc(sizeof(char)*20);              //Αυτή η κουραστική διαδικασία γίνεται γιατί θέλουμε να πιάνουν τα
    strcpy(city_table[0],"Athens");                     //strings λιγότερη μνήμη κατά την υπόλοιπη εκτέληση.
    city_table[0]=realloc(city_table[0],sizeof(char)*(strlen(city_table[0])+1));
    city_table[1]=malloc(sizeof(char)*20);
    strcpy(city_table[1],"Thessaloniki");
    city_table[1]=realloc(city_table[1],sizeof(char)*(strlen(city_table[1])+1));
    city_table[2]=malloc(sizeof(char)*20);
    strcpy(city_table[2],"Trikala");
    city_table[2]=realloc(city_table[2],sizeof(char)*(strlen(city_table[2])+1));
    city_table[3]=malloc(sizeof(char)*20);
    strcpy(city_table[3],"Volos");
    city_table[3]=realloc(city_table[3],sizeof(char)*(strlen(city_table[3])+1));
    city_table[4]=malloc(sizeof(char)*20);
    strcpy(city_table[4],"Lamia");
    city_table[4]=realloc(city_table[4],sizeof(char)*(strlen(city_table[4])+1));
/*-----------------------------------------------------------------*/ //Αρχικοποίηση πτήσεων
    for(i = 0; i < FLIGHT_COUNT; i++)
    {
        flights[i].available_seats = rand()%5 + 1;
        flights[i].flight_id = i+1;
        flights[i].from_city = malloc(sizeof(char)*20);
        strcpy(flights[i].from_city, city_table[rand()%5]);
        flights[i].from_city=realloc(flights[i].from_city,sizeof(char)*(strlen(flights[i].from_city)+1));
        flights[i].to_city = malloc(sizeof(char)*20);
        do
        {
            strcpy(flights[i].to_city, city_table[rand()%5]);
        } while (strcmp(flights[i].to_city,flights[i].from_city)==0);
        flights[i].to_city=realloc(flights[i].to_city,sizeof(char)*(strlen(flights[i].to_city)+1));
        flights[i].price = (float)(rand()%501);
    }

/*-----------------------------------------------------------------*/ //Επικοινωνία
    signal(SIGINT,exithandler);
    signal(SIGQUIT,exithandler);

    internalerror = listen(socket_fd, CLIENT_NUM);
    if (internalerror < 0)
	{
		perror("listen");
        freeallmem();
		exit(ERR_EXIT_CODE);
	}

	j=0;
    while(j<CLIENT_NUM)
    {
	    if ((client_fd = accept(socket_fd, (saddress *)&address,(socklen_t*)&addrlen)) < 0)
	    {
		    perror("Failed to accept request accept");
            freeallmem();                       //Δεν μπορούμε να αφήσουμε κανένα memory leak αν δεν τερματιστεί σωστα το πρόγραμμα.
		    exit(ERR_EXIT_CODE);
	    }

	    //rsize = read( client_fd , toreceive, sizeof(int));
	    //send(client_fd , tosend , sizeof(int) , 0 );
//        *tosend=FLIGHT_COUNT;
//         send(client_fd , tosend , sizeof(int) , 0 );
//         for(i = 0; i<FLIGHT_COUNT; i++)
//         {
//             *tosend=flights[i].flight_id;
//             send(client_fd , tosend , sizeof(int) , 0 );
//             *tosend=flights[i].available_seats;
//             send(client_fd , tosend , sizeof(int) , 0 );
//             strcpy(tosendtext,flights[i].from_city);
//             send(client_fd , tosendtext , sizeof(char)*20 , 0 );
//             strcpy(tosendtext,flights[i].to_city);
//             send(client_fd , tosendtext , sizeof(char)*20 , 0 );
//             *tosendfloat=flights[i].price;
//             send(client_fd , tosendfloat , sizeof(float) , 0 );
//         }
        read( client_fd , toreceive, sizeof(int));      //Θα μπορούσε να γίνει έλεγχος για το αν διακόπηκε η μεταφορά αν
                                                        //αυτο που επιστρέφει η read είναι διαφορετικής τιμής με την sizeof(int)
                                                        //αλλά δουλεύει σε local ip οπότε δεν έχουμε κάποιο θέμα.
        ticketcount=*toreceive;
        for(i = 0; i<ticketcount; i++)
        {
            read( client_fd , toreceive, sizeof(int));
            orno=*toreceive;
            *tosend=chkavail(&flights[orno]);
            flightprofitsum += flights[orno].price*(*tosend);
            flightrequests[orno]++;
            flightsuccess[orno]+=(*tosend);
            flightrequestssum++;
            flightsuccesssum+=(*tosend);
            write(client_fd , tosend , sizeof(int));
        }
        j++;
    }


/*-----------------------------------------------------------------*/  //Εκτύπωση και απελευθέρωση μνήμης
    exithandler();

    return 0;
}

int chkavail(flight * flp)
{
    //usleep(500000); //0.5 seconds = 500 miliseconds = 500000 microseconds
    sleep(1);
    if(flp->available_seats==0)
    {
        return 0;
    }
    flp->available_seats--;
    return 1;
}

void printflight(flight* fly)
{
    printf("-----------------------------\n");
    printf("Flight id: %d\n", fly->flight_id);
    printf("Availiable seats: %d\n", fly->available_seats);
    printf("City of departure: %s\n", fly->from_city);
    printf("City of arrival: %s\n", fly->to_city);
    printf("Price: %f\n", fly->price);
    printf("-----------------------------\n");
}
// https://www.youtube.com/watch?v=Ag1AKIl_2GM
void exithandler()
{
    int i;

/*-----------------------------------------------------------------*/ //Εκτύπωση στοιχείων
    printf("The flights after the clients' orders are as follows:\n");
    for(i = 0; i < FLIGHT_COUNT; i++)
    {
        printflight(&flights[i]);
        printf("Number of requests for the flight: %d\n", flightrequests[i]);
        printf("Number of successful requests: %d\n", flightsuccess[i]);
        printf("Number of unsuccessful requests: %d\n", flightrequests[i] - flightsuccess[i]);
        printf("-----------------------------\n");
    }
    printf("Overall:\n");
    printf("Total requests for flights: %d\n", flightrequestssum);
    printf("Total successful requests: %d\n", flightsuccesssum);
    printf("Total income: %f\n", flightprofitsum);
    freeallmem();
    exit(0);
}

void freeallmem()
{
    int i;
    for(i=0; i<FLIGHT_COUNT; i++)       //Δεν μπορούμε να αφήσουμε κανένα memory leak αν δεν τερματιστεί σωστα το πρόγραμμα.
    {
        free(flights[i].from_city);
        free(flights[i].to_city);
    }
    for(i=0; i< 5; i++)
    {
        free(city_table[i]);
    }
    free(toreceive);
    free(tosend);
    free(tosendfloat);
    free(tosendtext);
}

void exitterm()
{
    freeallmem();
    exit(1);
}
