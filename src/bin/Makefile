OBJS := dm5408_demo.o

SOURCE := dm5408_demo.c
OUTPUT := dm5408_demo
CFLAGS := -I../include -I../include -I/usr/include/dm5408 -I/usr/local/include/dm5408 -L../lib -L/usr/lib -ldm5408

CC ?= gcc

default : $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCE)

clean:
	@rm -f *.o $(OUTPUT)

install:
	@cp $(OUTPUT) /usr/local/bin/
	@chmod 555 /usr/local/bin/$(OUTPUT)

uninstall:
	@rm -f /usr/local/bin/$(OUTPUT)
