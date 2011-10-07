OBJS := libdm5408.o

SOURCE := libdm5408.c
OUTPUT := libdm5408.so
CFLAGS := -I../include -I/usr/include/dm5408 -I/usr/local/include/dm5408 -shared -fPIC

CC ?= gcc

all : $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCE)

clean:
	@rm -f *.so *.o

install :
	@cp $(OUTPUT) /usr/lib/$(OUTPUT)

uninstall:
	@rm -f /usr/lib/$(OUTPUT)
