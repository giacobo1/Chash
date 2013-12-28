CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lpthread
SOURCES=teste.cpp libhash.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=teste

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run:
	./teste


clear:
	rm *o teste