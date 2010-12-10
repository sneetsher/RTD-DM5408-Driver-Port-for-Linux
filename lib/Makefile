OBJS = dm5408_lib.o
CC = gcc
CFLAGS = -shared -fPIC
SOURCE = dm5408_lib.c
OUTPUT = dm5408_lib.so

default : $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCE)

install :
	cp $(OUTPUT) /lib/lib$(OUTPUT)

clean:
	rm *.so *.o
