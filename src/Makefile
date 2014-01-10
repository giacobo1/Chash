CC=g++
CFLAGS=-c -O3
LDFLAGS=-lpthread -lm
SOURCES=teste.cpp libhash.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o teste $(LDFLAGS)
	
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

test:
	./teste $(x) $(m) $(n) $(op) 

clear_o:
	rm *o 

clear_exe:
	rm teste

clear_all:
	rm *o teste