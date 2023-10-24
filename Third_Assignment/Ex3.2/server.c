#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <wait.h>
//#include <math.h>                 θα το εβαζα αλλα ο gcc θελει να κανω link την βιβλιοθηκη. Ενα απλό flag χρειαζεται στο τελος αλλα ας το αφησουμε.

#define PORT 1337
#define ERR_EXIT_CODE 1
#define CALCULATION 1
#define NUMBER 0

typedef struct sockaddr_in saddress_in;
typedef struct sockaddr saddress;


/*-----------------------------------------------------------------*/ //Δηλώσεις συναρτήσεων

void exithandler();
void freeallmem();
void exitterm();
int getres(char*, double*);
int isOpChar(char);
int string2num(char*,double*);
int char2num(char);
void removechar(char**, char);
void shiftleft(char**);
int findChar(char*,char);
double pow(double, double);
void wait4kid();
int findCharMulti(char* , char , int** );

/*-----------------------------------------------------------------*/ //Global bois


//char* toreceivetext;


/*-----------------------------------------------------------------*/ //Η main
int main(int argc, char** argv)
{
    double tosenddouble;
    double a;
    double b;
    int toreceiveint;
    int tosendint;
    int socket_fd;
    int client_fd;
    int internalerror;
    saddress_in address;
    int addrlen = sizeof(address);
    int rsize;
    int stopforking;
    int frez;
    stopforking=0;

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

    signal(SIGTERM,exitterm);
    signal(SIGINT,exitterm);        //Επειδή τώρα δηλώνουμε την μνήμη και δεν θέλουμε memory leaks, να κλείνει σωστα η εφαρμογή
    signal(SIGQUIT,exitterm);       //στην περίεργη περίπτωση που είναι κανείς γρήγορος να την κλείσει.

    internalerror = listen(socket_fd, SOMAXCONN);
    if (internalerror < 0)
    {
        perror("listen");
        freeallmem();
        exit(ERR_EXIT_CODE);
    }
    //printf("islistening\n");

    while(!stopforking)
    {
        if ((client_fd = accept(socket_fd, (saddress *)&address,(socklen_t*)&addrlen)) < 0)
        {
            perror("Failed to accept request accept");
            freeallmem();                       //Δεν μπορούμε να αφήσουμε κανένα memory leak αν δεν τερματιστεί σωστα το πρόγραμμα.
            exit(ERR_EXIT_CODE);
        }
        frez = fork();
        if(frez==0)
        {

            //printf("hasaccepted\n");
            stopforking=1;

            do
            {
                rsize = read( client_fd , &a, sizeof(double));
                rsize = read( client_fd , &b, sizeof(double));
                rsize = read( client_fd , &toreceiveint, sizeof(int));
                //toreceivetext=malloc(sizeof(char)*(toreceiveint + 1));
                //read(client_fd, toreceivetext, sizeof(char)*(toreceiveint + 1));
                //printf("got: %d | %s\n", toreceiveint,toreceivetext);
                //tosendint=getres(toreceivetext,&tosenddouble);
                if(toreceiveint==0)
                {
                    tosenddouble= a+b;
                    tosendint=0;
                }
                else if(toreceiveint==1)
                {
                    tosenddouble= a-b;
                    tosendint=0;
                }
                else if(toreceiveint==2)
                {
                    tosenddouble= a*b;
                    tosendint=0;
                }
                else if(toreceiveint==3)
                {
                    if(b==0)
                    {
                        tosendint=2;
                        tosenddouble=0;
                    }
                    else
                    {
                        tosenddouble= a/b;
                        tosendint=0;
                    }
                }
                else if(toreceiveint==4)
                {
                    tosenddouble= (double)((int)a%(int)b);
                    tosendint=0;
                }
                else if(toreceiveint==5)
                {
                    tosenddouble= pow(a,(int)b);
                    tosendint=0;
                }
                else
                {
                    tosendint=1;
                    tosenddouble=0;
                }
                //printf("sending: %d, %lf\n", tosendint, tosenddouble);

                write(client_fd , &tosendint , sizeof(int));
                write(client_fd , &tosenddouble , sizeof(double));
                rsize = read( client_fd , &toreceiveint, sizeof(int));
            }while(!toreceiveint);

        }
        else if(frez<0)
        {
            perror("error forking");
        }
        else
        {
            signal(SIGCHLD,wait4kid);
        }
    }
/*-----------------------------------------------------------------*/  //Ελευθέρωση της μνήμης.
    //Δεν προκειται να τρέξει αυτός ο κώδικας σε καμία περίπτωση but just in case
    freeallmem();   //Μετά απο πολλές αλλαγές τελικά δεν στελνουμε κείμενο αρα ειμαστε οκ...

    return 0;
}

void wait4kid()
{
    wait(NULL);
}
// https://www.youtube.com/watch?v=Ag1AKIl_2GM
void exithandler()
{
    freeallmem();
    exit(0);
}

void freeallmem()
{
    //free(toreceivetext);
}

void exitterm()
{
    freeallmem();
    exit(1);
}

int getres(char* inp, double* result)
{
    int i;
    char* tmp;
    char opchar;
    int tmpint;
    int tmpc;
    int tmplen;
    int opposs;
    int founderr;
    int keynum;
    int* keytab;
    char* a;
    char* b;
    double ad;
    double bd;
    printf("i work!\n");
    tmp = strdup(inp);
    removechar(&tmp,' ');
    removechar(&tmp,'\n');
    tmplen=strlen(tmp);
    opposs=-1;
    founderr=0;
    *result=0;

    opchar='+';
    keynum=findCharMulti(tmp,opchar,&keytab);
    if(keynum!=1)
    {
        founderr=1;
    }
    else if(keytab[0]==0 || keytab[0]==tmplen)
    {
        founderr=1;
    }
    else if(char2num(tmp[keytab[0]-1])>-1 || char2num(tmp[keytab[0]+1])>-1)
    {
        opposs=keytab[0];
    }
    else
    {
        founderr=1;
    }
    if(opposs<0 && founderr==0)
    {
        opchar='*';
        keynum=findCharMulti(tmp,opchar,&keytab);
        if(keynum!=1)
        {
            founderr=1;
        }
        else if(keytab[0]==0 || keytab[0]==tmplen)
        {
            founderr=1;
        }
        else if(char2num(tmp[keytab[0]-1])>-1 || char2num(tmp[keytab[0]+1])>-1)
        {
            opposs=keytab[0];
        }
        else
        {
            founderr=1;
        }
    }
    if(opposs<0 && founderr==0)
    {
        opchar='/';
        keynum=findCharMulti(tmp,opchar,&keytab);
        if(keynum!=1)
        {
            founderr=1;
        }
        else if(keytab[0]==0 || keytab[0]==tmplen)
        {
            founderr=1;
        }
        else if(char2num(tmp[keytab[0]-1])>-1 || char2num(tmp[keytab[0]+1])>-1)
        {
            opposs=keytab[0];
        }
        else
        {
            founderr=1;
        }
    }
    if(opposs<0 && founderr==0)
    {
        opchar='%';
        keynum=findCharMulti(tmp,opchar,&keytab);
        if(keynum!=1)
        {
            founderr=1;
        }
        else if(keytab[0]==0 || keytab[0]==tmplen)
        {
            founderr=1;
        }
        else if(char2num(tmp[keytab[0]-1])>-1 || char2num(tmp[keytab[0]+1])>-1)
        {
            opposs=keytab[0];
        }
        else
        {
            founderr=1;
        }
    }
    if(opposs<0 && founderr==0)
    {
        opchar='^';
        keynum=findCharMulti(tmp,opchar,&keytab);
        if(keynum!=1)
        {
            founderr=1;
        }
        else if(keytab[0]==0 || keytab[0]==tmplen)
        {
            founderr=1;
        }
        else if(char2num(tmp[keytab[0]-1])>-1 || char2num(tmp[keytab[0]+1])>-1)
        {
            opposs=keytab[0];
        }
        else
        {
            founderr=1;
        }
    }
    if(opposs<0 && founderr==0)
    {
        opchar='-';
        keynum=findCharMulti(tmp,opchar,&keytab);
        if(keynum>3)
        {
            founderr=1;
        }
        else if(keynum>0)
        {
            tmpint=0;
            if(keynum==3||keynum==2)
            {
                tmpint=1;
            }
            if(keytab[tmpint]==tmplen)
            {
                founderr=1;
            }
            else if(keytab[tmpint]>0)
            {
                if((tmp[keytab[tmpint]+1]=='-' && char2num(tmp[keytab[tmpint]-1])>-1)||(char2num(tmp[keytab[tmpint]+1])>-1 && char2num(tmp[keytab[tmpint]-1])>-1))
                {
                    opposs=keytab[tmpint];
                }
            }
        }
    }

    if(opposs>-1 && founderr==0)
    {
        a=malloc(sizeof(char)*(opposs+1));
        strncpy(a,tmp,opposs);
        if( string2num(a,&ad) == 1 )
        {
            founderr=1;
        }
        b=malloc(sizeof(char)*(tmplen-opposs+1));
        for(i=opposs; i<tmplen+1; i
            ++)
        {
            b[i]=tmp[i];
        }
        if( string2num(b,&bd) == 1 )
        {
            founderr=1;
        }
        if(opchar=='+')
        {
            *result= ad+bd;
        }
        if(opchar=='-')
        {
            *result= ad-bd;
        }
        if(opchar=='/')
        {
            if(bd==0)
            {
                founderr=2;
            }
            else
            {
                *result= ad/bd;
            }
        }
        if(opchar=='*')
        {
            *result= ad*bd;
        }
        if(opchar=='%')
        {
            *result= (int)ad%(int)bd;
        }
        if(opchar=='^')
        {
            *result= pow(ad,bd);
        }
        free(a);
        free(b);
    }
    free(tmp);
    return founderr;
}

int isOpChar(char inp)
{
    switch(inp)
    {
        case '+':
            return 1;
            break;
        case '-':
            return 2;
            break;
        case '/':
            return 1;
            break;
        case '*':
            return 1;
            break;
        case '%':
            return 1;
            break;
        case '^':
            return 1;
            break;
        default:
            return 0;
    }
    return 0;
}

int string2num(char* inp,double* outp)
{
    int len;
    int i;
    int passedDecPoint;
    double res;
    double temp;
    int error;
    error=0;
    passedDecPoint=0;
    len = strlen(inp);
    for(i=0; i<len; i++)
    {
        if(inp[i]=='.')
        {
            passedDecPoint=1;
            res = res/pow((double)10,(double)i);
        }
        else if(char2num(inp[i])>=0)
        {
            if(!passedDecPoint)
            {
                res=char2num(inp[i])*pow(10,len-i);
            }
            else
            {
                res=char2num(inp[i])*pow(10,-len+i);
            }
        }
        else
        {
            error = 1;
        }
    }
    *outp=res;
    return error;
}

int char2num(char inp)
{
    if(47 < inp && inp < 58)
    {
        return inp-48;      //0=48 στο ascii
    }
    else
    {
        return -1;
    }
}

void removechar(char** inp,char torem)
{
    char* a;
    char* b;
    char* tmp;
    int lena;
    int tmplen;
    int initlen=strlen(*inp);
    tmplen=initlen;
    a=malloc(sizeof(char));
    tmp=strdup(*inp);
    do
    {

        b=strchr(tmp,torem);

        lena=(tmplen - strlen(b));
        printf("removechar:i work!\n");
        a=realloc(a,sizeof(char)*(lena+1));
        shiftleft(&b);
        strncpy(a,*inp,lena);
        a[lena]='\0';
        strcat(a,b);
        strcpy(tmp,a);
        tmplen--;
        printf("removechar: tmplen is: %d\n", tmplen);
    }while(findChar(tmp,torem)>=0);
    *inp=strdup(tmp);
    free(tmp);
    free(a);
}

void shiftleft(char** inp)
{
    int i;
    char* tmp;
    int len = strlen(*inp)+1;
    tmp = *inp;
    for(i=1; i<len; i++)
    {
        tmp[i-1]=tmp[i];
    }
    (*inp)=realloc(*inp,sizeof(char)*(len-1));
    strcpy(*inp,tmp);
}

int findChar(char* inp, char key)
{
    int i;
    int len=strlen(inp);
    for(i=0;i<len;i++)
    {
        if(inp[i]==key)
        {
            return i;
        }
    }
    return -1;
}

int findCharMulti(char* inp, char key, int** points)
{
    int i,j;
    int len=strlen(inp);
    int* tmp;
    int pointlen;
    free(*points);
    *points=NULL;
    pointlen=0;
    for(i=0;i<len;i++)
    {
        if(inp[i]==key)
        {
            pointlen++;
            if(*points==NULL)
            {
                *points=malloc(sizeof(int)*pointlen);
            }
            else
            {
                tmp=malloc(sizeof(int)*pointlen);
                for(j=0; j<pointlen-1; j++)
                {
                    tmp[j]=(*points)[j];
                }
                (*points)=realloc(*points,sizeof(int)*pointlen);
                for(j=0; j<pointlen-1; j++)
                {
                    (*points)[j]=tmp[j];
                }
                free(tmp);
            }
            (*points)[pointlen-1]=i;
        }
    }
    return pointlen;
}

double pow(double num, double power)
{
    if(power==0)
    {
        return 1;
    }
    return num*pow(num,power-1);
}
