CC = g++
CFLAGS = -Wall
DEPS = headers.h
OBJ = fileClass.o sha.o utils.o handlers.o client.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

client: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -pthread -lssl -lcrypto -I/usr/include/openssl
 
clean:
	rm -rf *o client