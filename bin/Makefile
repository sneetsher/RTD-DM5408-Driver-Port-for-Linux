OBJS = dm5408_demo.o
CC = gcc
CFLAGS = -L../lib -ldm5408_lib
SOURCE = dm5408_demo.c
OUTPUT = dm5408_demo

default : $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCE)

clean:
	rm *.o $(OUTPUT)
