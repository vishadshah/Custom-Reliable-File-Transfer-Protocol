# Pavan Athreya Narasimha Murthy
# USC Id: 9129210968
# E-Mail: pavanatn@usc.edu, pavan.athreya@icloud.com
# EE542 - Fall 2018 - Project 5
# Make File to compile the two cpp files
# File 1: sender.c
# File 2: receiver.c

#################### START OF SHELL SCRIPT FOR "NODE B" ####################

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

#cleaning existing code files
echo "Cleaning object files"
make clean

echo "nodeB Script started"

#Compile the sender and receiver
echo "Compiling custom protocol source code"
make
echo "Code compilation complete"
timestamp

echo "Starting all transceivers"
####################### TRANSCEIVER START #######################
./receiver 1 2100 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3100 &
./receiver 2 2200 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3200 &
./receiver 3 2300 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3300 &
./receiver 4 2400 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3400 &
./receiver 5 2500 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3500 &
./receiver 6 2600 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3600 &
./receiver 7 2700 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3700 &
./receiver 8 2800 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3800 &
./receiver 9 2900 nodeA.lab5-pheonix.ee542.isi.deterlab.net 3900 &
./receiver 10 3000 nodeA.lab5-pheonix.ee542.isi.deterlab.net 4000 &&
wait
####################### TRANSCEIVER END #######################
echo "Reception of file completed successfully"
timestamp

echo "Merging all chunks into one file"
#Stitch all the files with .usc extension back into one .bin file
cat x1.usc x2.usc x3.usc x4.usc x5.usc x6.usc x7.usc x8.usc x9.usc x10.usc > data.bin
md5sum data.bin > /users/usc542ch/Pavan/Lab5/checksum_nodeb.txt
echo "File merge completed and checksum generated"
timestamp

echo "Running MD5 to verify the original and transmitted file"

if [ /users/usc542ch/Pavan/Lab5/checksum_nodeb.txt = /users/usc542ch/Pavan/Lab5/checksum_nodea.txt ]
then
    echo "Files have the same content"
else
    echo "Files have NOT the same content"
fi

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

rm data.bin

echo "******------GOOD BYE------******"

##########################################################################################