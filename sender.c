// Pavan Athreya Narasimha Murthy
// USC Id: 9129210968
// E-Mail: pavanatn@usc.edu, pavan.athreya@icloud.com
// EE542 - Fall 2018 - Project 5
// Make File to compile the two cpp files
// File 1: sender.c
// File 2: receiver.c

//Sender side of the custom file transfer protocol
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/time.h>

//Struct for the Protocol
typedef struct packet
{
	int SequenceNumber;
	char value[61682];
	bool sent;
}Packet;

//Array to hold missing sequence numbers
int MissedSequenceNumbers[1700];
int currentSequenceNumber = 0;
int senderId = 0;
int missedSequenceIndex = 0;

//Char array to hold other char arrays which are split for the 100MB file
struct packet fileParts[1700];

unsigned int lenUDP;
unsigned int lenTCP;
int n;

//Method to print the error and exit the program abnormally
void error(char *errorMsg);
//Send Packet
void SendPacket(int i);
void SendPacket(int i);
//Uses the network library to send packet to server
void FirePacket();
//End prgram method
void EndProgram();
void SendNextBatch();
void ReadFileandIndexLines(char *fileName);
void SetupUDPTransfer();
void SetupTCPReception();
void StartTransfer();
void WaitForTCPPacket();
void ProcessTCPPacket();
bool DidSendAllPackets();

unsigned int portno;
int sockfdUDP;
int sockfdTCP;
int newSockFdTCP;
struct sockaddr_in serv_addrUDP;
struct sockaddr_in serv_addrTCP;
struct sockaddr_in client_address;
struct hostent *server;

struct timeval start, end, timeout;

int portTCP;

////
//	Main Method - Entry point of the c program
////
int main(int argc, char *argv[])
{
	printf("Sender program: %s started sending data to %s at port number %s using file: %s\n", argv[1], argv[3], argv[2], argv[4]);

	//250 ms round trip time
	timeout.tv_sec = 2;
	timeout.tv_usec = 250000; //micro seconds

	//Set the sender ID
	senderId = atoi(argv[1]);
	//Setting port number
	portno = atoi(argv[2]);
	//Parse IP address
	server = gethostbyname(argv[3]);
    if (server == NULL)
    {
        error("ERROR, no such host\n");
    }
	//Process File
	ReadFileandIndexLines(argv[4]);

    portTCP = atoi(argv[5]);

	//clock to check the processing time of the request
	gettimeofday(&start, NULL);

	SetupUDPTransfer();
	
	SetupTCPReception();

	memset(&MissedSequenceNumbers, 0, sizeof(int) * 1700);

	StartTransfer();

	return 0;
}

void ReadFileandIndexLines(char *fileName)
{
	printf("Reading file: %s\n",fileName);
	FILE *fp;
	fp = fopen(fileName,"r");
	if(NULL != fp)
	{
		int i = 0;
		char line[61682];
		while(fgets(line, 61682, fp))
		{
//			Packet *pack = (Packet *)malloc(sizeof(Packet));
//			memset(&pack,0,sizeof(pack));
//			pack->SequenceNumber = i;
			fileParts[i].SequenceNumber = i;
			strncpy(fileParts[i].value, line, 61682);
			fileParts[i].sent = false;
//			fileParts[i] = *pack;
			i++;
			memset(&line, 0, (sizeof(char) * 61682));
			//printf("Index: %d",i);
		}
		printf("Total Number of structs created: %d\n", i);
		fclose(fp);	
	}
	else
	{
		error("Unable to open file");
	}
	printf("Parsing the file into character arrays complete\n");
    printf("End time in UNIX: %lu\n", (unsigned long)time(NULL));
}

void SetupUDPTransfer()
{
	//create datagram socket
	sockfdUDP = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfdUDP < 0)
	{
		error("ERROR creating socket, Please close the terminal window and try again\n");
	}

	if (setsockopt (sockfdUDP, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
    	error("ERROR setting socket timeout.\n");
    }

	bcopy((char *)server->h_addr, (char *)&serv_addrUDP.sin_addr.s_addr, server->h_length);

	//Address family
	serv_addrUDP.sin_family = PF_INET;
	//Setting the receivers IP Address
	//serv_addrUDP.sin_addr.s_addr = inet_addr("10.0.0.2");
	//Changing the endienness to network endian (Big endian)
	serv_addrUDP.sin_port = htons(portno);

	lenUDP = sizeof(serv_addrUDP);
}

void SetupTCPReception()
{
	//create datagram socket
	sockfdTCP = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfdTCP < 0)
	{
		error("ERROR creating socket, Please close the terminal window and try again\n");
	}

	if (setsockopt (sockfdTCP, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
    	error("ERROR setting socket timeout.\n");
    }

	//Address family
	serv_addrTCP.sin_family = PF_INET;
	//Setting the receivers IP Address
	serv_addrTCP.sin_addr.s_addr = INADDR_ANY;
	//Changing the endienness to network endian (Big endian)
	serv_addrTCP.sin_port = htons(portTCP);

	if (bind(sockfdTCP, (struct sockaddr *)&serv_addrTCP, sizeof(serv_addrTCP)) < 0)
	{
		error("Error on binding the TCP sockets");
	}

	if (listen(sockfdTCP, 5) < 0)
	{
		error("Error on listening to TCP sockets");
	}

	lenTCP = sizeof(serv_addrTCP);

	unsigned int clientLength = sizeof(client_address);
	newSockFdTCP = accept(sockfdTCP, (struct sockaddr *) &client_address, &clientLength);
	if (newSockFdTCP < 0)
	{
		error("Error on accepting packets using TCP");
	}
}

void StartTransfer()
{
	printf("Start time in UNIX: %lu\n", (unsigned long)time(NULL));
	//while(1)
	//{
		//Send 4 packets
		SendNextBatch();
//		if (DidSendAllPackets() == true)
//		{
//			break;
//		}
		//Wait for TCP with missing sequences
		WaitForTCPPacket();
		//Add them to MissedSequenceNumbers
		ProcessTCPPacket();
	//}
	EndProgram();
}

bool DidSendAllPackets()
{
	for (int i = 0; i < 1700; ++i)
	{
		if (fileParts[i].sent == false)
		{
			return false;
		}
	}
	return true;
}

void SendNextBatch()
{
	int limit = 0;
	int i = 0;
	for (i = 0; i < 1700; i++)
	{
//		if ((fileParts[i].sent == false) && (limit < 1700))
//		{
//			memset(&buffer,0,sizeof(buffer));
//			buffer.SequenceNumber = fileParts[i].SequenceNumber;
//			strncpy(buffer.value, fileParts[i].value, 32769);
			SendPacket(i);
//			currentSequenceNumber++;
//			limit++;
//		}
	}
}

void WaitForTCPPacket()
{
	int n;
	n = read(sockfdTCP,MissedSequenceNumbers,sizeof(int) *1700);
	if (n < 0)
	{
		//error("Error reading from TCP socket");
		printf("Error reading from TCP socket\n");
	}
}

void ProcessTCPPacket()
{
	//add the sequence number to array
	for(int i = 0; i < 1700; i++)
	{
		if (0 != MissedSequenceNumbers[i])
		{
			fileParts[i].sent = true;
		}
	}
	memset(&MissedSequenceNumbers, 0, sizeof(int) * 1700);
}

////
//	Send Character
////
void SendPacket(int i)
{
	//printf("Sending sequence number: %d\n", fileParts[i].SequenceNumber);
	FirePacket(i);
	FirePacket(i);
	FirePacket(i);
}

////
//	Uses the network library to send packet to server
////
void FirePacket(int i)
{
	Packet buffer = fileParts[i];
    n = sendto(sockfdUDP, &buffer, sizeof(buffer), 0, (struct sockaddr *)&serv_addrUDP, lenUDP);
    if (n < 0)
    {
    	error("ERROR sending to server.\n");
    } 
}

////
//  End Program
////
void EndProgram()
{
	//clock to check processing time of the request
	gettimeofday(&end, NULL);
	double time_spent = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000000.0f;
	printf("Sender: %dProcessing Time: %0.6f seconds\n", senderId, time_spent);
	close(sockfdUDP);
	close(sockfdTCP);
	close(newSockFdTCP);
	printf("\nSender: %d execution complete\n", senderId);
	exit(0);
}

////
//	Method to print the error and abnormally exit the program
////
void error(char *errorMsg)
{
	//Print the error message
	printf("%s\n", errorMsg);
	printf("Exiting the program\n");
	//exit the process
	exit(1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
