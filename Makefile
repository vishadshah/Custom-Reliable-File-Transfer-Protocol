# Pavan Athreya Narasimha Murthy
# USC Id: 9129210968
# E-Mail: pavanatn@usc.edu, pavan.athreya@icloud.com
# EE542 - Fall 2018 - Project 5
# Make File to compile the two cpp files
# File 1: sender.c
# File 2: receiver.c	

#Creates the executable of sender and receiver if there are changes in object file
output: receiver.o sender.o
	gcc receiver.o -o receiver
	gcc sender.o -o sender

#Compiles the sender and receiver code if there are new changes
output.o: sender.c receiver.c
	gcc -c sender.c
	gcc -c receiver.c

#Deletes all the compiled and executable files
clean:
	rm *.o receiver sender

# format of makefile
# target: dependencies
# 	action