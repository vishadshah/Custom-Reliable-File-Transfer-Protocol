Pavan Athreya Narasimha Murthy
USC Id: 9129210968
E-Mail: pavanatn@usc.edu, pavan.athreya@icloud.com
EE542 - Fall 2018 - Project 5
Make File to compile the two cpp files
File 1: sender.c
File 2: receiver.c
File 3: lab5nodea.sh
File 4: lab5nodeb.sh

Project5 - Reliable transport protocol better than scp

"Synopsis of each file and its part in the project"

File 1: sender.c
Briefly explain in steps on how the sender works
- The sender implements ReadFileIndexLines() method which splits the 100MB file into smaller packets of 1MB which are sent to the receiver on nodeB using UDP protocol.
- The UDP connection is set up in SetupUDPTransfer() method.
- The packets are sent using a UDP socket connection in FirePacket() method.
- To make sure all packets are sent, TCP socket connection is used to receive ACKs from the receiver. The TCP connection is setup in SetupTCPReception() method.
- To send the next batch of packets once the previous batch is acknowlegded, we use SendNextBatch().
- To wait for TCP missing sequences and add them to the missed sequence number, we use StartTransfer() method.
-The waiting for missing sequence number is implemented in WaitForTCPPacket() method and adding them to the list of missed sequence number is implemented in ProcessTCPPacket() method.
- EndProgram implements closing all socket connections and gracefully completing the execution.

File 2: receiver.c
Briefly explain in steps on how the receiver works
- The receiver setups the UDP reception to get the packets in SetupUDPReception() method.
- It setups the TCP connection to send back the Sequence numbers of the packets received ot the sender using SetupTCPTransfer() method.
- It implements StartReceiving() method for receiving the UDP packets and to send back the sequence number to the sender.
- It sends the TCP packets with sequence numbers to the sender using SendReceivedPacketsToSender() method.
- EndProgram implements closing all socket connections and gracefully completing the execution.

File 3: lab5nodea.sh
Explain what is going on in the script at nodeA
- To display the time when we start the program, we use timestamp() method.
- To create a 1GB file with random data, we use dd function.
- We send md5 code to receiver using scp/whatever way.
- To split the 1GB file into smaller parts of 100MB, we use split function.
- We compile the sender using make and run the sender.
- We wait till all the sender programs send the 100MB file using "wait" function.


File 4: lab5nodeb.sh
Explain what is going on in the script at nodeB
- To display the time when we start the program, we use timestamp() method.
- To compile the receiver code, we use make after which we run them.
- We merge the 10 files into one using cat function
- We compare the new and old file by running the md5
- We remove all the unwanted files using rm and then calculate the throughput.