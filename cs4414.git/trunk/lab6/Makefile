CC=gcc
FLAGS=-g -o

make: forensics.o
	$(CC) $(FLAGS) lab6 forensics.o
	./lab6 datafile.le

out:
	head -n 20 out.txt
	head -n 20 nodes.txt

raw:
	hexdump -x < datafile.le > raw.txt

clean:
	-rm -f *.o *~ lab6 out.txt raw.txt

delete:
	-rm -f file.pdf file.gif file.tiff file.ps
