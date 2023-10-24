CC = g++
CFLAGS = -Wall
DEPS = headers.h  
OBJ = Classes/userClass.o Classes/groupClass.o Classes/fileClass.o utils.o commands.o handlers.o tracker.o  
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

tracker: $(OBJ)
	$(CC) $(CFLAGS) -pthread -o $@ $^ 

clean:
	rm -rf *o tracker