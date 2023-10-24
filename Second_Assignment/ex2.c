#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define CLIENT_NUM 10
#define FLIGHT_COUNT 10
/*###############################################################*/

typedef struct fl{ 
    int flight_id; 
    char* from_city;
    char* to_city;
    int available_seats;
    float price;
}flight;

/*---------------------------------------------------------------*/

void childfunc(int serverfds[2],int clientfds[2]);//, int);
void fatherfunc();
int chkavail(flight*);
void printflight(flight*);

/*###############################################################*/ 

int main(int argc, char** argv)
{
    srand(time(NULL));
    //sleep(5);
    fatherfunc();
    //printf("%c%c%c\n",115,117,115);     //some sussy code ඞ
    return 0;
}

/*###############################################################*/

void childfunc(int serverfds[2],int clientfds[2])//, int id)   // το id ειναι μονο για debug λόγους
{   
    int i;
    int orcount=0;
    int avail=0;
    int rnumber;
    int res;
    //sleep(1);
    //printf("[CLIENT %d ]: Client is running\n", id);
    //srand(time(NULL));
    orcount = rand()%3 + 1;
    //printf("[CLIENT %d ]: Sending number of orders (%d)\n", id, orcount);
    //close(serverfds[READ]);
    write (serverfds[WRITE], &orcount ,sizeof(int));
    //printf("[CLIENT %d ]: orcount after sending attempt: %d\n",id , orcount);
    for(i = 0; i < orcount; i++)
    {
        srand(time(NULL));                      //Κανονικά η srand ειναι για την αρχή του προγράμματος αλλά, λόγω
                                                //της ψευδοτυχαιότητάς της βγάζει συνέχεια ίδιες τιμές, άρα
                                                //επαναρχηκοποιούμε για νέα τιμή βάσει χρόνου.
        rnumber = rand()%FLIGHT_COUNT;
        //printf("[CLIENT %d ]: Sending order %d of %d with content %d\n",id, i, orcount, rnumber);
        //close(serverfds[READ]);
        write(serverfds[WRITE],&rnumber ,sizeof(int));
        //close(clientfds[WRITE]);
        read(clientfds[READ], &res, sizeof(int));   //Μπορούμε να βγάλουμε απο τα σχόλια τον κώδικα για να δουμε την τιμή
                                                    //που πήρε η res (το αποτέλεσμα). H read δουλεύει κάνοντας πιο ρεαλιστική
                                                    //την εκτέλεση καθώς θα θέλει και ο client να ξέρει αν ήταν επιτυχής η κράτηση
        /*
        if(res)
        {
            printf("[CLIENT %d ]: Order received fulfillable\n", id);
        }
        else
        {
            printf("[CLIENT %d ]: Order received unfulfillable\n", id);
        }
        */
    }
    //close(serverfds[READ]);
    //close(serverfds[WRITE]);
    //close(clientfds[WRITE]);
    //close(clientfds[READ]);
    //printf("[CLIENT %d ]: Exiting client\n", id);
}

/*###############################################################*/

void fatherfunc()
{
    flight flights[FLIGHT_COUNT];
    int flightrequests[FLIGHT_COUNT];
    int flightsuccess[FLIGHT_COUNT];
    float flightprofitsum;
    int flightrequestssum;
    int flightsuccesssum;
    char * city_table[5];
    int i,j;
    int pid;
    int stop_forking;
    int ticketcount;
    int orno;
    int chkintp;
    int childrtrncd;
    int serverfds[CLIENT_NUM][2];
    int clientfds[CLIENT_NUM][2];
/*-----------------------------------------------------------------*/ //Αρχηκοποίηση μεταβλητών
    flightrequestssum = 0;
    flightsuccesssum = 0;
    flightprofitsum = 0;
    for (i=0; i<FLIGHT_COUNT; i++)
    {
        flightrequests[i]=0;
        flightsuccess[i]=0;
    }
    for (i=0; i<CLIENT_NUM; i++)
    {
        pipe(serverfds[i]);
        pipe(clientfds[i]);
    }
    pid = getpid();
    //printf("[SERVER %d ]: Server is running\n", pid);
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
    //printf("[SERVER %d ]: Availiable flights are:\n", pid);
    /*
    for(i = 0; i < FLIGHT_COUNT; i++)
    {
        printflight(&flights[i]);
    }
    */
/*-----------------------------------------------------------------*/ //Δημιουργία των clients
    stop_forking = 0;
    i = 0;
    while(i < CLIENT_NUM && !stop_forking)
    {
        //printf("[SERVER %d ]: Forking is going to %d client\n", pid, i);
        //sleep(1);
        pid = fork();
        if(pid == 0)
        {
            childfunc(serverfds[i],clientfds[i]);//, i);
            stop_forking = 1;                               //Εδώ χρειάζεται να φύγουμε για να μήν κανουν τα παιδιά
                                                            //δικά τους παιδιά!
        }
        else if (pid == -1)
        {
            fprintf(stderr,"[SERVER %d ]: Error while forking\n", pid);
        }
        i++;
    }
/*-----------------------------------------------------------------*/ //Διαδοχική επικοινωνία με τους clients
    if(pid != 0 )
    {
        //printf("[SERVER %d ]: Forking complete \n", pid);
        for (i = 0; i < CLIENT_NUM; i++)
        {
            //sleep(1);
            //printf("[SERVER %d ]: Checking client %d.\n", pid, i);
            ticketcount = 0;
            orno = 0;
            close(clientfds[i][READ]);
            close(serverfds[i][WRITE]);
            read(serverfds[i][READ], &ticketcount, sizeof(int));
            //printf("[SERVER %d ]: ticketcount received: %d (client %d)\n", pid, ticketcount, i);
            j = 0;
            while (j < ticketcount)
            {
                //printf("[SERVER %d ]: Fulfilling order with number: %d (client %d)\n", pid, j, i);
                //close(serverfds[i][WRITE]);
                read(serverfds[i][READ], &orno, sizeof(int));
                //printf("[SERVER %d ]: Client asked for order with number: %d (client %d)\n", pid, orno, i);
                //printf("[SERVER %d ]: The order above is the following: %d (client %d)\n", pid, i);
                //printflight(&flights[orno]);
                chkintp = chkavail(&flights[orno]);
/*.................................................................*/
                flightprofitsum += flights[orno].price*chkintp;
                flightrequests[orno]++;
                flightsuccess[orno]+=chkintp;
                flightrequestssum++;
                flightsuccesssum+=chkintp;
/*.................................................................*/
                //printf("[SERVER %d ]: Sending %d to client: (client %d)\n", pid, chkintp, i);
                //close(clientfds[i][READ]);
                write(clientfds[i][WRITE], &chkintp, sizeof(int));
                j++; 
            }
            wait(&childrtrncd);
            close(serverfds[i][READ]);
            close(serverfds[i][WRITE]);
            close(clientfds[i][WRITE]);
            close(clientfds[i][READ]);
            
        }
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
        //system("mpv https://www.youtube.com/watch?v=Ag1AKIl_2GM 1> /dev/null 2> /dev/null");
    }
    for(i=0; i<FLIGHT_COUNT; i++)
    {
        free(flights[i].from_city);
        free(flights[i].to_city);
    }
    for(i=0; i< 5; i++)
    {
        free(city_table[i]);
    }
    
}

/*###############################################################*/

int chkavail(flight * flp)
{
    usleep(500000); //0.5 seconds = 500 miliseconds = 500000 microseconds
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