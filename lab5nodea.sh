# Pavan Athreya Narasimha Murthy
# USC Id: 9129210968
# E-Mail: pavanatn@usc.edu, pavan.athreya@icloud.com
# EE542 - Fall 2018 - Project 5
# Make File to compile the two cpp files
# File 1: sender.c
# File 2: receiver.c
# File 3: transceiver.c

#################### START OF SHELL SCRIPT FOR "NODE A" ####################

#!/bin/sh

#Start

#Clear Screen
clear

#mv ee542lab5.zip /tmp/
#cd /tmp/
#unzip ee542lab5.zip
#cd ee542lab5/
#rm -rf __MACOSX

#Initialise Time
SECONDS=0

#Method to print time stamp in HH:mm:ss
timestamp() {
  date +"%T"
}

#Should print unix timestamp when we start...
#either take the program time stamp or the script time stamp

#cleaning existing code files
echo "Cleaning object files"
make clean

echo "nodeA Script started"

#Create a 100MB file with random data
#dd if=/dev/urandom of=data.bin bs=1M count=1024
#md5sum data.bin > /users/usc542ch/Pavan/Lab5/checksum_nodea.txt
echo "file created and md5 checksum saved in ~/Pavan/Lab5/checksum.txt"
timestamp

#send md5 to the other ndie using whatever way possible
#store the checksum in into a file and then SCP

#Split file into pieces of 1MB each and append .usc format
echo "Splitting files into smaller chunks"
#split data.bin -a 1 -b 104857600 --additional-suffix=.usc
cp 100MB.txt xa.usc
cp 100MB.txt xb.usc
cp 100MB.txt xc.usc
cp 100MB.txt xd.usc
cp 100MB.txt xe.usc
cp 100MB.txt xf.usc
cp 100MB.txt xg.usc
cp 100MB.txt xh.usc
cp 100MB.txt xi.usc
cp 100MB.txt xj.usc
echo "Splitting files complete"
timestamp
echo "File names created"
ls ./*.usc

echo "Compiling custom protocol source code"
make

#Compile the sender and receiver
echo "Code compilation complete"
timestamp

echo "Waiting for receivers to start"
sleep 5

echo "Starting all senders"
####################### SENDER START #######################
./sender 1 2100 nodeB.lab5-pheonix.ee542.isi.deterlab.net xa.usc 3100 &
./sender 2 2200 nodeB.lab5-pheonix.ee542.isi.deterlab.net xb.usc 3200 &
./sender 3 2300 nodeB.lab5-pheonix.ee542.isi.deterlab.net xc.usc 3300 &
./sender 4 2400 nodeB.lab5-pheonix.ee542.isi.deterlab.net xd.usc 3400 &
./sender 5 2500 nodeB.lab5-pheonix.ee542.isi.deterlab.net xe.usc 3500 &
./sender 6 2600 nodeB.lab5-pheonix.ee542.isi.deterlab.net xf.usc 3600 &
./sender 7 2700 nodeB.lab5-pheonix.ee542.isi.deterlab.net xg.usc 3700 &
./sender 8 2800 nodeB.lab5-pheonix.ee542.isi.deterlab.net xh.usc 3800 &
./sender 9 2900 nodeB.lab5-pheonix.ee542.isi.deterlab.net xi.usc 3900 &
./sender 10 3000 nodeB.lab5-pheonix.ee542.isi.deterlab.net xj.usc 4000 &&
wait
echo "Files transfered successfully"
####################### SENDER END #######################
#Update the seconds elapsed
duration=$SECONDS
echo "Number of seconds ($duration)"
timestamp

#Remove all unwanted files
echo "Deleting smaller files"
rm *.usc
echo "Small data files removed"
timestamp

#Update the seconds elapsed
duration=$SECONDS
echo "Number of seconds ($duration)"
#echo "Throughput: $(((1024*1024*1024*8) / duration))"
echo "$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."

#m data.bin
#rm ee542lab5.zip
echo "******------GOOD BYE------******"