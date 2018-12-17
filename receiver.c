// Pavan Athreya Narasimha Murthy
// USC Id: 9129210968
// E-Mail: pavanatn@usc.edu, pavan.athreya@icloud.com
// EE542 - Fall 2018 - Project 5
// Make File to compile the two cpp files
// File 1: sender.c
// File 2: receiver.c

//Receiver side of custom file transfer protocol
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/time.h>

//Method to print the error and exit the program abnormally
void error(char *errorMsg);

//Expected sequence
int reqNum;

//Struct for GBN UDP Protocol
typedef struct packet
{
    int SequenceNumber;
    char value[61682];
    bool sent;
}Packet;

//Array to hold missing sequence numbers
int ReceivedSequenceNumbers[1700];
int currentSequenceNumber = 0;
int transceiverId = 0;
int missedSequenceIndex = 0;
bool shouldBreak = false;

//Char array to hold other char arrays which are split for the 100MB file
struct packet fileParts[1700];

//String input form the user
Packet buffer;

unsigned int lenUDP;
unsigned int lenTCP;
unsigned int clientLegth;
int n;

//End prgram method
void EndProgram(void);
void SetupUDPReception();
void SetupTCPTransfer();
void StartReceiving();
bool DidReceiverAllData();
void ReceiveUDPPackets();
void SendReceivedPacketsToSender();
void WritePacketsToFile();

unsigned int portno;
int sockfdUDP;
int sockfdTCP;
int newSockFdTCP;
int ReceiverId;
struct sockaddr_in serv_addrUDP;
struct sockaddr_in serv_addrTCP;
struct sockaddr_in client_address;
struct hostent *server;

struct timeval start, end, timeot;

int numberReceived = 0;

int portTCP;

////
//    Main Method - Entry point of the c program
////
int main(int argc, char *argv[])
{
    printf("Receiver program: %s started receiving data from %s at port number %s\n", argv[1], argv[3], argv[2]);

    //200 ms round trip time
    timeot.tv_sec = 10;
    timeot.tv_usec = 250000; //micro seconds

    //Set the sender ID
    ReceiverId = atoi(argv[1]);
    //Setting port number
    portno = atoi(argv[2]);
    //Parse IP address
    server = gethostbyname(argv[3]);
    if (server == NULL)
    {
        error("ERROR, no such host\n");
    }

    portTCP = atoi(argv[4]);
    
    //clock to check the processing time of the request
    gettimeofday(&start, NULL);

    SetupUDPReception();
    
    SetupTCPTransfer();

    memset(&ReceivedSequenceNumbers, 0, sizeof(int) * 1700);
    
    StartReceiving();
    
    return 0;
}

void SetupUDPReception()
{
    //Creating the socket
    sockfdUDP = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfdUDP < 0)
    {
        error("ERROR opening socket, Please close the terminal window and try again");
    }

    if (setsockopt (sockfdUDP, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeot, sizeof(timeot)) < 0) {
        error("ERROR setting socket time out.\n");
    }

    //Address family
    serv_addrUDP.sin_family = PF_INET;
    //Setting the IP address to loopback address
    serv_addrUDP.sin_addr.s_addr = INADDR_ANY;
    //changing the endienness to network endian (Big endian)
    serv_addrUDP.sin_port = htons(portno);

    //Binding the socket with port number and IP address
    if (bind(sockfdUDP, (struct sockaddr *) &serv_addrUDP,sizeof(serv_addrUDP)) < 0)
    {
        error("ERROR on binding, Please close the terminal window and try again");
    }

    clientLegth = sizeof(client_address);
    printf("\nServer is running and ready to receive connections on port %d\n", portno);
}

void SetupTCPTransfer()
{
    sockfdTCP = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfdTCP < 0)
    {
        error("ERROR opening socket, Please close the terminal window and try again");
    }

    bcopy((char *)server->h_addr_list[0], (char *)&serv_addrTCP.sin_addr.s_addr, server->h_length);
    
    //Address family
    serv_addrTCP.sin_family = PF_INET;
    //Setting the receivers IP Address
    //serv_addrTCP.sin_addr.s_addr = inet_addr(argv[3]);
    //Changing the endienness to network endian (Big endian)
    serv_addrTCP.sin_port = htons(portTCP);
    
    if (connect(sockfdTCP, (struct sockaddr *) &serv_addrTCP, sizeof(serv_addrTCP)) < 0)
    {
        error("Error connecting");
    }
    
    lenTCP = sizeof(serv_addrTCP);
}


void StartReceiving()
{
    ReceiveUDPPackets();
    if(DidReceiverAllData())
    {
        //printf("Should write to file now\n");
        WritePacketsToFile();
        printf("done writing to file\n");
        EndProgram();
    }
    StartReceiving();
}

void WritePacketsToFile()
{
    char *fileName = (char *)malloc(sizeof(char) * 7);
    sprintf(fileName, "x%d.usc",ReceiverId);
    FILE *file = fopen(fileName,"w");
	for (int i = 0; i < 1700; ++i)
	{
		int result = fputs(fileParts[i].value,file);
		if (EOF == result)
        {
		    error("Error writing to the file");
        }
	}
	fclose(file);
}

bool DidReceiverAllData()
{
    //printf("Did receive all data: ");
    for (int i = 0; i < 1700; i++)
    {
        if (fileParts[i].sent == false)
        {
            //printf("NO\n");
            return false;
        }
    }
    //printf("YES\n");
    return true;
}

void ReceiveUDPPackets() {
    while (1)
    {
        int n;
        memset(&buffer, 0, sizeof(buffer));
        n = recvfrom(sockfdUDP, &buffer, sizeof(buffer), 0, (struct sockaddr *) &client_address, &clientLegth);
        if (n < 0) {
            //error("ERROR receiving from sender.\n");
            printf("ERROR receiving from sender: probably timeout\n");
            SendReceivedPacketsToSender();
            break;
        } else {
            Packet pack = (Packet) buffer;
            fileParts[pack.SequenceNumber].SequenceNumber = pack.SequenceNumber;
            strncpy(fileParts[pack.SequenceNumber].value, pack.value, 61682);
            fileParts[pack.SequenceNumber].sent = true;
            ReceivedSequenceNumbers[pack.SequenceNumber] = pack.SequenceNumber;
            //printf("Received Sequence Number: %d\n", pack.SequenceNumber);
        }
        if(DidReceiverAllData())
        {
            gettimeofday(&end, NULL);
            double time_spent = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000000.0f;
            printf("Processing Time: %0.6f seconds\n", time_spent);
            printf("End time in UNIX: %lu\n", (unsigned long)time(NULL));
            break;
        }
    }
}

void SendReceivedPacketsToSender()
{
    //printf("Sending back acknowledgements\n");
    int n;
    n = write(sockfdTCP,ReceivedSequenceNumbers,sizeof(int)*10);
    if (n < 0)
    {
        error("Error writing to TCP socket");
    }
}

////
//  End Program
////
void EndProgram()
{
    //clock to check processing time of the request
//    gettimeofday(&end, NULL);
//    double time_spent = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000000.0f;
//    printf("Processing Time: %0.6f seconds\n", time_spent);
    close(sockfdUDP);
    close(sockfdTCP);
    close(newSockFdTCP);
    printf("\nTransceiver: %d execution complete\n", transceiverId);
    exit(0);
}

////
//Method to print the error and abnormally exit the program
////
void error(char *errorMsg)
{
    //Print the error message
    printf("\n %s" ,errorMsg);
    printf("\n Exiting Program\n");
    //exit the process
    exit(1);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
